#ifndef CUBE_HPP
#define CUBE_HPP
#include <cinttypes>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <Windows.h>

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
		default: throw std::invalid_argument("No such color: " + colour);
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
			std::vector<Colour> colours(colour_map.size());
			size_t i = 0;
			for (char symbol : colour_map) {
				colours[i] = Colour(symbol);
			}
			return colours;
		}
		catch (std::exception const& e) {
			std::cerr << "Failed to recognize the symbol from colour map\n" << e.what();
			return std::vector<Colour>();
		}
	}
}


class RubiksCube {
public:
	enum class Faces : uint8_t {
		UP, LEFT, FRONT, RIGHT, BACK, DOWN
	};
	enum class Edges : uint8_t {
		UP_LEFT, UP_BACK, UP_RIGHT, UP_FRONT,
		LEFT_FRONT, LEFT_BACK, RIGHT_BACK, RIGHT_FRONT,
		DOWN_LEFT, DOWN_BACK, DOWN_RIGHT, DOWN_FRONT
	};
	enum class Corners : uint8_t {
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
		centers_.resize(FACES_COUNT);
		edges_.resize(EDGES_COUNT);
		corners_.resize(CORNERS_COUNT);
	}
	RubiksCube(std::initializer_list<CenterCubie> const& centers, std::initializer_list<EdgeCubie> const& edges, \
		std::initializer_list<CornerCubie> const& corners) : centers_(centers), edges_(edges), corners_(corners) {
		centers_.resize(FACES_COUNT);
		edges_.resize(EDGES_COUNT);
		corners_.resize(CORNERS_COUNT);
	}
	explicit RubiksCube(std::string colour_map)
		: centers_(std::vector<CenterCubie>(FACES_COUNT)), 
		edges_(std::vector<EdgeCubie>(EDGES_COUNT)), 
		corners_(std::vector<CornerCubie>(CORNERS_COUNT)) {
		try {
			colour_map.erase(std::remove_if(colour_map.begin(), colour_map.end(), std::isspace), colour_map.end());
			std::vector<Colour> colours = CubieHelper::to_colours(colour_map);


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

	void setup(std::string const& scramble) {}
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




	void validate() {

	}
};


namespace CubeHelper {
	RubiksCube generate_cube() {
		RubiksCube cube;

		return cube;
	}
}
#endif
