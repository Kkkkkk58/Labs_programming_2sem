#ifndef CUBE_HPP
#define CUBE_HPP
#include <cinttypes>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <Windows.h>
#include <regex>
#include "CubeExceptions.hpp"


constexpr uint8_t COLOURS_COUNT = 6;
constexpr uint8_t FACES_COUNT = 6;
constexpr uint8_t EDGES_COUNT = 12;
constexpr uint8_t CORNERS_COUNT = 8;
constexpr uint8_t STICKERS_COUNT = 54;

class Colour {
public:
	enum class Colours : uint8_t {
		WHITE = 2,
		BLUE = 4,
		RED = 8,
		GREEN = 16,
		YELLOW = 32,
		ORANGE = 64
	};
	Colour(Colours colour = Colours::WHITE) : colour_(static_cast<uint8_t>(colour)) {}
	Colour(char colour) {
		switch (colour) {
		case 'W': case 'w':
			colour_ = static_cast<uint8_t>(Colours::WHITE);
			break;
		case 'B': case 'b':
			colour_ = static_cast<uint8_t>(Colours::BLUE);
			break;
		case 'R': case 'r':
			colour_ = static_cast<uint8_t>(Colours::RED);
			break;
		case 'G': case 'g':
			colour_ = static_cast<uint8_t>(Colours::GREEN);
			break;
		case 'Y': case 'y':
			colour_ = static_cast<uint8_t>(Colours::YELLOW);
			break;
		case 'O': case 'o':
			colour_ = static_cast<uint8_t>(Colours::ORANGE);
			break;
		default: throw UnknownColour(std::string(1, colour));
		}
	}
	~Colour() = default;
	Colour(Colour const& other) : colour_(other.colour_) {}
	void swap(Colour& other) {
		using std::swap;
		swap(colour_, other.colour_);
	}
	Colour(Colour &&other) noexcept : colour_(std::move(other.colour_)) {}
	Colour& operator=(Colour const& other) {
		if (this != &other) {
			colour_ = other.colour_;
		}
		return *this;
	}
	Colour& operator=(Colour&& other) noexcept {
		swap(other);
		return *this;
	}
	Colour& operator+=(Colour const& other) {
		colour_ |= other.colour_;
		return *this;
	}
	bool operator==(Colour const& other) const {
		return colour_ == other.colour_;
	}
	std::vector<Colours> get_state() const {
		std::vector<Colours> colours;
		uint8_t bit_mask = static_cast<uint8_t>(Colours::WHITE);
		for (size_t i = 0; i < COLOURS_COUNT; ++i) {
			if (colour_ & bit_mask) {
				colours.push_back(static_cast<Colours>(bit_mask));
			}
			bit_mask = bit_mask << 1;
		}
		return colours;
	}

	//void print_first_matched(std::ostream& os) const {
	//	std::vector<Colours> state = get_state();

	//	os << static_cast<Colours>(state[0]);
	//}
private:
	uint8_t colour_;
};

Colour operator+(Colour lhs, Colour const& rhs) {
	return lhs += rhs;
}

std::ostream& operator<<(std::ostream& os, Colour::Colours const& colour) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (colour) {
	case Colour::Colours::WHITE:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
		os << 'W'; 
		break;
	case Colour::Colours::BLUE: 
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x1);	
		os << 'B'; 
		break;
	case Colour::Colours::GREEN: 
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x2);
		os << 'G'; 
		break;
	case Colour::Colours::ORANGE: 
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0xE);
		os << 'O';
		break;
	case Colour::Colours::RED: 
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x4);
		os << 'R'; 
		break;
	case Colour::Colours::YELLOW:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x6);
		os << 'Y'; 
		break;
	}
	SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
	return os;
}


class Cubie {
public:
	virtual Colour get_colour() const = 0;
	virtual ~Cubie() {}
};

class EdgeCubie : public Cubie {
public:
	enum class Orientation : uint8_t {
		CORRECT = 0,
		INVERTED = 1
	};
	EdgeCubie() 
		: Cubie(), colour_(Colour(Colour::Colours::YELLOW) + Colour(Colour::Colours::BLUE)), orientation_(Orientation::CORRECT) {}
	EdgeCubie(Colour const& a, Colour const& b, Orientation orientation = Orientation::CORRECT)
		: Cubie(), colour_(a + b), orientation_(orientation) {}

	EdgeCubie(EdgeCubie const& other) : colour_(other.colour_), orientation_(other.orientation_) {}
	void swap(EdgeCubie& other) {
		using std::swap;
		swap(colour_, other.colour_);
		swap(orientation_, other.orientation_);
	}
	EdgeCubie(EdgeCubie&& other) noexcept {
		swap(other);
	}
	EdgeCubie& operator=(EdgeCubie const& other) {
		if (this != &other) {
			colour_ = other.colour_;
			orientation_ = other.orientation_;
		}
		return *this;
	}
	EdgeCubie& operator=(EdgeCubie&& other) noexcept {
		swap(other);
		return *this;
	}
	Colour get_colour() const override {
		return colour_;
	}
	Orientation get_orientation() const {
		return orientation_;
	}
private:
	Colour colour_;
	Orientation orientation_;
};


class CornerCubie : public Cubie {
public:
	enum class Orientation : uint8_t {
		CORRECT = 0,
		CLOCKWISE = 1,
		COUNTERCLOCKWISE = 2
	};
	CornerCubie()
		: Cubie(), colour_(Colour(Colour::Colours::WHITE) + Colour(Colour::Colours::BLUE) + Colour(Colour::Colours::RED)),
		orientation_(Orientation::CORRECT) {}
	CornerCubie(Colour const& a, Colour const& b, Colour const& c, Orientation orientation = Orientation::CORRECT) : Cubie(), 
		colour_(a + b + c), orientation_(orientation) {}

	CornerCubie(CornerCubie const& other) : colour_(other.colour_), orientation_(other.orientation_) {}
	void swap(CornerCubie& other) {
		using std::swap;
		swap(colour_, other.colour_);
		swap(orientation_, other.orientation_);
	}
	CornerCubie(CornerCubie&& other) noexcept {
		swap(other);
	}
	CornerCubie& operator=(CornerCubie const& other) {
		if (this != &other) {
			colour_ = other.colour_;
			orientation_ = other.orientation_;
		}
		return *this;
	}
	CornerCubie& operator=(CornerCubie&& other) noexcept {
		swap(other);
		return *this;
	}
	Colour get_colour() const override {
		return colour_;
	}
	Orientation get_orientation() const {
		return orientation_;
	}

private:
	Colour colour_;
	Orientation orientation_;
};


class CenterCubie : public Cubie {
public:
	explicit CenterCubie(Colour const& colour = Colour::Colours::WHITE) : Cubie(), colour_(colour) {}
	CenterCubie(CenterCubie const& other) : colour_(other.colour_) {}
	void swap(CenterCubie& other) {
		using std::swap;
		swap(colour_, other.colour_);
	}
	CenterCubie(CenterCubie&& other) noexcept {
		swap(other);
	}
	CenterCubie& operator=(CenterCubie const& other) {
		if (this != &other) {
			colour_ = other.colour_;
		}
		return *this;
	}
	CenterCubie& operator=(CenterCubie&& other) noexcept {
		swap(other);
		return *this;
	}
	Colour get_colour() const override {
		return colour_;
	}
private:
	Colour colour_;
};


namespace CubieHelper {
	std::vector<Colour> to_colours(std::string const& colour_map) {
		try {
			std::vector<uint8_t> number_check(COLOURS_COUNT);
			std::vector<Colour> colours(colour_map.size());
			size_t i = 0;
			for (char symbol : colour_map) {
				colours[i] = Colour(symbol);
				uint8_t colour_index = static_cast<uint8_t>((colours[i].get_state())[0]);
				if (number_check[colour_index] >= 9) {
					throw InvalidState("Too many stickers of the same colour");
				}
				number_check[colour_index] += 1;
			}
			return colours;
		}
		catch (std::exception const& e) {
			std::cerr << "Failed to recognize the colour map\n" << e.what();
			return std::vector<Colour>();
		}
	}
}

class RubiksCube;

class CubeValidator {
public:
	enum class StickersNumberCheck : bool {
		DISABLED, ENABLED
	};
	CubeValidator(RubiksCube const& cube) : cube_(cube) {}
	std::string report(StickersNumberCheck const& option = StickersNumberCheck::ENABLED) {
		// TODO: Run multithreading
		if (option == StickersNumberCheck::ENABLED && !correct_stickering) {
			return "Incorrect stickering";
		}
		if (!edges_invariant()) {
			return "Edges invariant failed";
		}
		if (!corners_invariant()) {
			return "Corners invariant failed";
		}
		if (!permutations_invariant()) {
			return "Permutations invariant failed";
		}
		return "OK";
	}
private:
	//TODO:
	bool correct_stickering() const {
		return true;
	}
	bool edges_invariant() const {
		return true;
	}
	bool corners_invariant() const {
		return true;
	}
	bool permutations_invariant() const {
		return true;
	}
	RubiksCube cube_;
};

class RubiksCube {
public:
	enum Faces : uint8_t {
		UP, LEFT, FRONT, RIGHT, BACK, DOWN
	};
	enum Edges : uint8_t {
		UP_LEFT, UP_BACK, UP_RIGHT, UP_FRONT,
		LEFT_FRONT, LEFT_BACK, RIGHT_BACK, RIGHT_FRONT,
		DOWN_LEFT, DOWN_BACK, DOWN_RIGHT, DOWN_FRONT
	};
	enum Corners : uint8_t {
		UP_LEFT_BACK, UP_RIGHT_BACK, UP_RIGHT_FRONT, UP_LEFT_FRONT,
		DOWN_LEFT_BACK, DOWN_RIGHT_BACK, DOWN_RIGHT_FRONT, DOWN_LEFT_FRONT
	};
	RubiksCube()
		: centers_(std::vector<CenterCubie>(FACES_COUNT)), 
		edges_(std::vector<EdgeCubie>(EDGES_COUNT)), 
		corners_(std::vector<CornerCubie>(CORNERS_COUNT)) {
		default_init();
	}
	RubiksCube(std::vector<CenterCubie> const& centers, std::vector<EdgeCubie> const& edges, \
		std::vector<CornerCubie> const& corners) : centers_(centers), edges_(edges), corners_(corners) {
		try {
			centers_.resize(FACES_COUNT);
			edges_.resize(EDGES_COUNT);
			corners_.resize(CORNERS_COUNT);
			validate();
		}
		catch (std::exception const& e) {
			std::cerr << "Failed to construct the Cube: " << e.what() << "\nCreating the default one";
			default_init();
		}
	}
	RubiksCube(std::initializer_list<CenterCubie> const& centers, std::initializer_list<EdgeCubie> const& edges, \
		std::initializer_list<CornerCubie> const& corners) : centers_(centers), edges_(edges), corners_(corners) {
		try {
			centers_.resize(FACES_COUNT);
			edges_.resize(EDGES_COUNT);
			corners_.resize(CORNERS_COUNT);
			validate();
		}
		catch (std::exception const& e) {
			std::cerr << "Failed to construct the Cube: " << e.what() << "\nCreating the default one";
			default_init();
		}
	}
	explicit RubiksCube(std::string colour_map)
		: centers_(std::vector<CenterCubie>(FACES_COUNT)), 
		edges_(std::vector<EdgeCubie>(EDGES_COUNT)), 
		corners_(std::vector<CornerCubie>(CORNERS_COUNT)) {
		try {
			colour_map.erase(std::remove_if(colour_map.begin(), colour_map.end(), std::isspace), colour_map.end());
			std::vector<Colour> colours = CubieHelper::to_colours(colour_map);
			if (colours.size() == 0) {
				default_init();
				return;
			}
			init_center(colours);
			// TODO: Multithreading
			init_edges(colours);
			init_corners(colours);
			validate(CubeValidator::StickersNumberCheck::DISABLED);

		}
		catch (std::exception const& e) {
			std::cerr << "Failed to construct custom Cube:\n" << e.what() << "\nCreating the default one\n";
			default_init();
			return;
		}
		
	}
	RubiksCube(RubiksCube const& other) : centers_(other.centers_), edges_(other.edges_), corners_(other.corners_) {}
	void swap(RubiksCube& other) {
		using std::swap;
		swap(centers_, other.centers_);
		swap(edges_, other.edges_);
		swap(corners_, other.corners_);
	}
	RubiksCube(RubiksCube&& other) noexcept
		: centers_(std::move(other.centers_)), edges_(std::move(other.edges_)), corners_(std::move(other.corners_)) {}
	RubiksCube& operator=(RubiksCube const& other) {
		if (this != &other) {
			centers_ = other.centers_;
			edges_ = other.edges_;
			corners_ = other.corners_;
		}
		return *this;
	}
	RubiksCube& operator=(RubiksCube&& other) noexcept {
		swap(other);
		return *this;
	}
	~RubiksCube() = default;
	bool is_solved() {
		return std::all_of(edges_.begin(), edges_.end(), \
			[](EdgeCubie const& edge) { return edge.get_orientation() == EdgeCubie::Orientation::CORRECT; })
			&& std::all_of(corners_.begin(), corners_.end(), \
				[](CornerCubie const& corner) {return corner.get_orientation() == CornerCubie::Orientation::CORRECT; });
	}

	void rotate(std::string const& scramble) {
		// TODO : Aho-Corasick ??
		std::regex command_template("([UuDdLlRrFfBbxXyYzZeEsSmM])([1-9]+)?(')?");
		std::sregex_iterator rit(scramble.begin(), scramble.end(), command_template);
		std::sregex_iterator rend;
	
		while (rit != rend) {
			char direction = std::string((*rit)[1])[0];
			uint8_t times = (std::string((*rit)[2]).length()) ? std::stoi((*rit)[2]) : 1;
			bool clockwise = (std::string((*rit)[3]).length() == 0) ? true : false;

			perform(direction, clockwise, times);
			++rit;
		}
	}
private:
	std::vector<CenterCubie> centers_;
	std::vector<EdgeCubie> edges_;
	std::vector<CornerCubie> corners_;

	void default_init() {
		using C = Colour::Colours;
		centers_ = { CenterCubie(C::WHITE), CenterCubie(C::ORANGE), CenterCubie(C::GREEN), \
					CenterCubie(C::RED), CenterCubie(C::BLUE), CenterCubie(C::YELLOW) };

		edges_ = { EdgeCubie(C::WHITE, C::ORANGE), EdgeCubie(C::WHITE, C::BLUE), EdgeCubie(C::WHITE, C::RED), EdgeCubie(C::WHITE, C::GREEN),
					EdgeCubie(C::GREEN, C::ORANGE), EdgeCubie(C::ORANGE, C::BLUE), EdgeCubie(C::BLUE, C::RED), EdgeCubie(C::RED, C::GREEN),
					EdgeCubie(C::ORANGE, C::YELLOW), EdgeCubie(C::BLUE, C::YELLOW), EdgeCubie(C::RED, C::YELLOW), EdgeCubie(C::GREEN, C::YELLOW) };

		corners_ = { CornerCubie(C::WHITE, C::ORANGE, C::BLUE), CornerCubie(C::WHITE, C::BLUE, C::RED),
					CornerCubie(C::WHITE, C::GREEN, C::RED), CornerCubie(C::WHITE, C::GREEN, C::ORANGE),
					CornerCubie(C::ORANGE, C::BLUE, C::YELLOW), CornerCubie(C::RED, C::BLUE, C::YELLOW),
					CornerCubie(C::RED, C::GREEN, C::YELLOW), CornerCubie(C::GREEN, C::ORANGE, C::YELLOW) };
	}


	void init_center(std::vector<Colour> const& colours) {
		for (size_t i = 4, j = 0; i < 50, j < FACES_COUNT; i += 9, ++j) {
			centers_[j] = CenterCubie(colours[i]);
		}
	}

	void init_corners(std::vector<Colour> const& colour) {
		std::vector<std::vector<uint8_t>> indexes = { {0, 38, 9}, {2, 29, 36}, {8, 20, 27}, {6, 11, 18},
													  {51, 15, 44}, {53, 42, 35}, {47, 33, 26}, {45, 24, 17} };
		Colour top_colour = centers_[UP].get_colour(), bottom_colour = centers_[DOWN].get_colour();
		
		for (uint8_t i = Corners::UP_LEFT_BACK; i <= Corners::DOWN_LEFT_FRONT; ++i) {
			
			CornerCubie::Orientation orientation;
			if (colour[indexes[i][0]] == top_colour || colour[indexes[i][0]] == bottom_colour) {
				orientation = CornerCubie::Orientation::CORRECT;
			}
			else if (colour[indexes[i][1]] == top_colour || colour[indexes[i][1]] == bottom_colour) {
				orientation = CornerCubie::Orientation::CLOCKWISE;
			}
			else {
				orientation = CornerCubie::Orientation::COUNTERCLOCKWISE;
			}

			corners_[i] = CornerCubie(colour[indexes[i][0]], colour[indexes[i][1]], colour[indexes[i][2]], orientation);
		}
	}

	void init_edges(std::vector<Colour> const& colour) {
		std::vector<std::vector<uint8_t>> indexes =   { {3, 10}, {1, 37}, {5, 28}, {7, 19},
														{14, 21}, {12, 41}, {32, 39}, {30, 23},
														{48, 16}, {52, 34}, {50, 34}, {46, 25} };
		for (uint8_t i = Edges::UP_LEFT; i <= Edges::DOWN_FRONT; ++i) {
			Faces checked_side, opposite_side;
			if (i <= Edges::UP_FRONT) {
				checked_side = Faces::UP;
				opposite_side = Faces::DOWN;
			}
			else if (i <= Edges::LEFT_BACK) {
				checked_side = Faces::LEFT;
				opposite_side = Faces::RIGHT;
			}
			else if (i <= Edges::RIGHT_FRONT) {
				checked_side = Faces::RIGHT;
				opposite_side = Faces::LEFT;
			}
			else {
				checked_side = Faces::BACK;
				opposite_side = Faces::FRONT;
			}

			EdgeCubie::Orientation orientation = (centers_[checked_side].get_colour() == colour[indexes[i][0]] ||
				centers_[opposite_side].get_colour() == colour[indexes[i][0]])\
				? EdgeCubie::Orientation::CORRECT : EdgeCubie::Orientation::INVERTED;
			edges_[i] = EdgeCubie(colour[indexes[i][0]], colour[indexes[i][1]], orientation);
		}
	}

	void perform(char direction, bool clockwise, uint8_t times) {
		switch (direction) {
		case 'x': case 'X':
			flip_x(clockwise, times);
			break;
		case 'y': case 'Y':
			flip_y(clockwise, times);
			break;
		case 'z': case 'Z':
			flip_z(clockwise, times);
			break;
		case 'u': case 'U':
			break;
		case 'd': case 'D':
			break;
		case 'l': case 'L':
			break;
		case 'r': case 'R':
			break;
		case 'f': case 'F':
			break;
		case 'b': case 'B':
			break;

		case 'm': case 'M':
			break;
		case 'e': case 'E':
			break;
		case 's': case 'S':
			break;
		}
	}
	void flip_x(bool clockwise, uint8_t times) {
		for (uint8_t i = 0; i < times; ++i) {
			if (clockwise) {
				centers_ = { centers_[Faces::FRONT], centers_[Faces::LEFT], centers_[Faces::DOWN],
							centers_[Faces::RIGHT], centers_[Faces::UP], centers_[Faces::BACK] };

			}
			else {
				centers_ = { centers_[Faces::BACK], centers_[Faces::LEFT], centers_[Faces::UP],
							centers_[Faces::RIGHT], centers_[Faces::DOWN], centers_[Faces::FRONT] };
			}

		}
	}
	void flip_y(bool clockwise, uint8_t times) {
		for (uint8_t i = 0; i < times; ++i) {
			if (clockwise) {
				std::rotate(centers_.begin() + LEFT, centers_.begin() + BACK, centers_.begin() + DOWN);
			}
			else {
				std::rotate(centers_.begin() + LEFT, centers_.begin() + FRONT, centers_.begin() + DOWN);
			}

		}
	}
	void flip_z(bool clockwise, uint8_t times) {
		for (uint8_t i = 0; i < times; ++i) {
			if (clockwise) {
				centers_ = { centers_[Faces::LEFT], centers_[Faces::DOWN], centers_[Faces::FRONT],
							centers_[Faces::UP], centers_[Faces::BACK], centers_[Faces::RIGHT] };
			}
			else {
				centers_ = { centers_[Faces::RIGHT], centers_[Faces::UP], centers_[Faces::FRONT],
							centers_[Faces::DOWN], centers_[Faces::BACK], centers_[Faces::LEFT] };
			}
		}
	}
	void validate(CubeValidator::StickersNumberCheck const& option = CubeValidator::StickersNumberCheck::ENABLED) {
		CubeValidator validator(*this);
		std::string check_results = validator.report(option);
		if (check_results != "OK") {
			throw InvalidState(check_results);
		}
	}


};

// Functor class??
namespace CubeHelper {
	RubiksCube generate_cube() {
		RubiksCube cube;

		return cube;
	}
}
#endif
