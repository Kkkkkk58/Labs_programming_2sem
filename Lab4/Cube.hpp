#ifndef CUBE_HPP
#define CUBE_HPP
#include <vector>
#include <array>
#include <string>
#include <iterator>
#include <Windows.h>
#include <regex>

class Cubie {
public:
	enum class Colours : char {
		WHITE, GREEN, YELLOW, BLUE, RED, ORANGE
	};
	Cubie() : colour_(Colours::WHITE) {}
	Cubie(Colours const& colour) : colour_(colour) {}
	Cubie(char colour) {
		switch (colour) {
		case 'W': case 'w': colour_ = Colours::WHITE; break;
		case 'G': case 'g': colour_ = Colours::GREEN; break;
		case 'Y': case 'y': colour_ = Colours::YELLOW; break;
		case 'B': case 'b': colour_ = Colours::BLUE; break;
		case 'R': case 'r': colour_ = Colours::RED; break;
		case 'O': case 'o': colour_ = Colours::ORANGE; break;
		default: throw std::invalid_argument("Wrong colour");
		}
	}
	Cubie(Cubie const& other) : colour_(other.colour_) {}
	void swap(Cubie& other) {
		using std::swap;
		swap(colour_, other.colour_);
	}
	Cubie(Cubie&& other) noexcept {
		swap(other);
	}
	Cubie& operator=(Cubie const& other) {
		if (this != &other) {
			colour_ = other.colour_;
		}
		return *this;
	}
	Cubie& operator=(Cubie&& other) noexcept {
		swap(other);
		return *this;
	}
	Colours colour() const { return colour_; }
	~Cubie() = default;
private:
	Colours colour_;
	//virtual ~Cubie() {}
	//virtual void turn(char direction) = 0;
};


std::ostream& operator<<(std::ostream& os, Cubie::Colours c) {
	//HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (c) {
	case Cubie::Colours::WHITE: /*SetConsoleTextAttribute(handle, FOREGROUND_BLUE);*/ os << "W"; break;
	case Cubie::Colours::GREEN: /*SetConsoleTextAttribute(handle, FOREGROUND_GREEN);*/ os << "G"; break;
	case Cubie::Colours::YELLOW: /*SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);*/ os << "Y"; break;
	case Cubie::Colours::RED: /*SetConsoleTextAttribute(handle, FOREGROUND_RED);*/os << "R"; break;
	case Cubie::Colours::BLUE: /*SetConsoleTextAttribute(handle, FOREGROUND_BLUE);*/ os << "B"; break;
	case Cubie::Colours::ORANGE: /*SetConsoleTextAttribute(handle, FOREGROUND_RED);*/ os << "O"; break;
		//case Cubie::Colours::BLACK: os << "*"; break;
	}
	return os;
}
//
//class CenterCubie : public Cubie {
//public:
//	CenterCubie(Cubie::Colours a) : colour(a) {}
//	Cubie::Colours colour;
//	void turn(char direction) {}
//};
//
//class EdgeCubie : public Cubie {
//public:
//	EdgeCubie(Cubie::Colours a, Cubie::Colours b, Cubie::Colours c, Cubie::Colours d, Cubie::Colours e, Cubie::Colours f) : colours({a, b, c, d, e, f}) {}
//	std::vector<Cubie::Colours> colours;
//	void turn(char direction) {
//		switch (direction) {
//		case 'r':
//			std::rotate(colours.begin() + 1, colours.begin() + 4, colours.begin() + 5);
//			break;
//		case 'l':
//			std::rotate(colours.rbegin() + 1, colours.rbegin() + 4, colours.rbegin() + 5);
//		case 'u':
//
//		case 'd':
//
//		//default:
//		//	break;
//		}
//	}
//};
//
//class CornerCubie : public Cubie {
//public:
//	CornerCubie(Cubie::Colours a, Cubie::Colours b, Cubie::Colours c, Cubie::Colours d, Cubie::Colours e, Cubie::Colours f) : colours({ a, b, c, d, e, f }) {}
//	std::vector<Cubie::Colours> colours;
//	void turn(char direction, bool clockwise = true) {
//
//	}
//};
//
class Face {
public:
	using Row = std::array<Cubie, 3>;
	enum class Type : char {
		UP, LEFT, FRONT, RIGHT, BACK, DOWN
	};
	Face() : cubies_(), type_(Type::DOWN) {}
	template<std::input_iterator InputIt>
	Face(InputIt first, InputIt last, Type type) : cubies_(first, last), type_(type) {}
	Face(std::array<std::array<Cubie, 3>, 3> const& cubies, Type type) : cubies_(cubies), type_(type) {}
	Face(Cubie ul, Cubie um, Cubie ur, Cubie ml, Cubie mm, Cubie mr, Cubie dl, Cubie dm, Cubie dr, Type type)
		: cubies_({ Row{ul, um, ur}, Row{ml, mm, mr}, Row{dl, dm, dr} }), type_(type) {}
	~Face() = default;
	Face(Face const& other) : cubies_(other.cubies_), type_(other.type_) {}
	void swap(Face& other) {
		using std::swap;
		swap(cubies_, other.cubies_);
		swap(type_, other.type_);
	}
	Face(Face&& other) noexcept : cubies_(std::move(other.cubies_)), type_(other.type_) {}
	Face& operator=(Face const& other) {
		if (this != &other) {
			cubies_ = other.cubies_;
			type_ = other.type_;
		}
		return *this;
	}
	Face& operator=(Face&& other) noexcept {
		swap(other);
		return *this;
	}
	//class Proxy_row {
	//public:
	//	template<std::input_iterator InputIt>
	//	Proxy_row(size_t i,  InputIt const& arr) : row_(arr, arr + 3) {}
	//	//
	//	Cubie const& operator[](size_t i) const {
	//		return row_[i];
	//	}
	//	operator std::array<Cubie, 3>() const {
	//		return row_;
	//	}
	//private:
	//	std::array<Cubie, 3> row_;
	//};
	////Proxy_row& operator[](size_t i) {
	////	return Proxy_row(i, &cubies_[i * 3]);
	////}
	//Proxy_row const& operator[](size_t i) const {
	//	return Proxy_row(i, &cubies_[i * 3]);
	//}
	Row& operator[](size_t i) {
		return cubies_[i];
	}
	Row const& operator[](size_t i) const {
		return cubies_[i];
	}
	Face inverted(bool vertical, bool same_order = false) {
		if (vertical) {
			return Face(std::array<Row, 3>{inverted_row(2), inverted_row(1), inverted_row(0)}, type_);
		}
		else if (!same_order) {
			return Face(std::array<Row, 3>{column(2), column(1), column(0)}, type_);
		}
		else {
			return Face(std::array<Row, 3>{inverted_column(0), inverted_column(1), inverted_column(2)}, type_);
		}
	}
	Cubie::Colours main_colour() const {
		return cubies_[1][1].colour();
	}
	void rotate(bool clockwise = true) {
		if (clockwise) {
			cubies_ = { Row{cubies_[2][0], cubies_[1][0], cubies_[0][0]}, \
						Row{cubies_[2][1], cubies_[1][1], cubies_[0][1]},
						Row{cubies_[2][2], cubies_[1][2], cubies_[0][2]} };
		}
		else {
			cubies_ = { Row{cubies_[0][2], cubies_[1][2], cubies_[2][2]}, \
						Row{cubies_[0][1], cubies_[1][1], cubies_[2][1]},
						Row{cubies_[0][0], cubies_[1][0], cubies_[2][0]} };
		}
	}
	void set_type(Type type) {
		type_ = type;
	}
	Type type() const {
		return type_;
	}
	bool is_solved() const {
		Cubie::Colours colour = main_colour();
		return std::all_of(cubies_.begin(), cubies_.end(), [&](Row const& r) {\
			return std::all_of(r.begin(), r.end(), [&](Cubie const& c) { return c.colour() == colour; });
			});
	}
	void read_sequence() {

	}
private:
	std::array<Row, 3> cubies_;
	Type type_;
	Row inverted_row(size_t i) {
		Row r = cubies_[i];
		std::reverse(r.begin(), r.end());
		return r;
	}
	Row inverted_column(size_t i) {
		return Row{ cubies_[2][i], cubies_[1][i], cubies_[0][i] };
	}
	Row column(size_t i) {
		return Row{ cubies_[0][i], cubies_[1][i], cubies_[2][i] };
	}

};


std::ostream& operator<<(std::ostream& os, Face::Type const& t) {
	switch (t) {
	case Face::Type::UP: os << "Up"; break;
	case Face::Type::DOWN: os << "Down"; break;
	case Face::Type::LEFT: os << "Left"; break;
	case Face::Type::RIGHT: os << "Right"; break;
	case Face::Type::FRONT: os << "Front"; break;
	case Face::Type::BACK: os << "Back"; break;
	}
	return os;
}


class Cube {
public:
	Cube() { default_init(); }
	template<std::input_iterator InputIt>
	Cube(InputIt first, InputIt last) : faces_(first, last) {
		if (!is_valid()) {
			throw std::invalid_argument("Invalid state");
		}
	}
	explicit Cube(std::array<Face, 6> const& faces) : faces_(faces) {
		if (!is_valid()) {
			throw std::invalid_argument("Invalid state");
		}
	}
	Cube(Face const& up, Face const& left, Face const& front, Face const& right, Face const& back, Face const& down)
		: faces_({ up, left, front, right, back, down }) {
		if (!is_valid()) {
			throw std::invalid_argument("Invalid state");
		}
	}
	Cube(std::string const& sequence) {
		size_t stri = 0;
		for (size_t i = 0; i < 6; ++i) {
			for (size_t j = 0; j < 3; ++j) {
				for (size_t k = 0; k < 3; ++k) {
					//std::cout << i * 6 + j * 3 + k << "\n";
					faces_[i][j][k] = sequence[stri];
					++stri;
				}
			}
		}
	}
	~Cube() = default;
	Cube(Cube const& other) : faces_(other.faces_) {}
	void swap(Cube& other) {
		using std::swap;
		swap(faces_, other.faces_);
	}
	Cube(Cube&& other) noexcept : faces_(std::move(other.faces_)) {}
	void flip(char axis, char direction, size_t times = 1) {
		times %= 4;

		switch (axis) {
		case 'Z':
			switch (direction) {
			case 'D':
				for (size_t i = 0; i < times; ++i) {
					flip_down();
				}
				break;
			case 'U':
				for (size_t i = 0; i < (4 - times); ++i) {
					flip_down();
				}
				break;
			}
			break;
		case 'Y':
			switch (direction) {
			case 'L':
				for (size_t i = 0; i < times; ++i) {
					flip_left_y();
				}
				break;
			case 'R':
				for (size_t i = 0; i < (4 - times); ++i) {
					flip_left_y();
				}
				break;
			}
			break;
		case 'X':
			switch (direction) {
			case 'L':
				std::cout << "FLIP\n";
				for (size_t i = 0; i < times; ++i) {
					flip_left_x();
				}
				break;
			case 'R':
				for (size_t i = 0; i < (4 - times); ++i) {
					flip_left_x();
				}
				break;
			}
			break;
		default:
			break;
		}
	}

	Face& operator[](size_t i) {
		return faces_[i];
	}
	Face const& operator[](size_t i) const {
		return faces_[i];
	}
	Face& operator[](char f) {
		switch (f) {
		case 'U': return faces_[UP];
		case 'L': return faces_[LEFT];
		case 'F': return faces_[FRONT];
		case 'R': return faces_[RIGHT];
		case 'B': return faces_[BACK];
		case 'D': return faces_[DOWN];
		default: throw std::invalid_argument("Unknown parameter");
		}
	}
	Face const& operator[](char f) const {
		switch (f) {
		case 'U': return faces_[UP];
		case 'L': return faces_[LEFT];
		case 'F': return faces_[FRONT];
		case 'R': return faces_[RIGHT];
		case 'B': return faces_[BACK];
		case 'D': return faces_[DOWN];
		default: throw std::invalid_argument("Unknown parameter");
		}
	}
	void rotate(char direction, bool clockwise = true, size_t times = 1) {
		switch (direction) {
		case 'U': case 'u':
		case 'D': case 'd':
			horizontal_move(direction, clockwise, times);
			break;
		case 'L': case 'l':
		case 'R': case 'r':
			side_move(direction, clockwise, times);
			break;
		case 'F': case 'f':
		case 'B': case 'b':
			facing_move(direction, clockwise, times);
			break;
		default: throw std::invalid_argument("Cannot perform this move");
		}
	}
	friend std::ostream& operator<<(std::ostream& os, Cube const& cube) {
		for (size_t i = 0; i < 3; ++i) {
			std::cout << "\n\t";
			for (size_t j = 0; j < 3; ++j) {
				std::cout << cube.faces_[0][i][j].colour() << " ";
			}
		}
		for (size_t i = 0; i < 3; ++i) {
			std::cout << "\n";
			//std::cout << face.type() << "\n";
			for (size_t j = 1; j < 5; ++j) {
				for (size_t k = 0; k < 3; ++k) {
					std::cout << cube.faces_[j][i][k].colour() << " ";
					if (k == 2) {
						std::cout << "  ";
					}
				}
			}
		}
		for (size_t i = 0; i < 3; ++i) {

			std::cout << "\n\t";
			for (size_t j = 0; j < 3; ++j) {
				std::cout << cube.faces_[5][2 - i][2 - j].colour() << " ";
			}
		}
		return os;
	}
	// TODO:
	//void print_scramble(std::ostream &os) {}
	//void print_scramble(std::string const& filename) {}
	//void read_scramble(std::ostream& os) {}
	//void read_scramble(std::string const& filename) {}
	//void read_sweep(std::ostream& os) {}
	//void read_sweep(std::string const& filename) {}
	//void read_sequence(std::ostream &os) {}
	bool is_solved() const {
		return std::all_of(faces_.begin(), faces_.end(), [](Face const& f) { return f.is_solved(); });
	}

	friend std::istream& operator>>(std::istream& is, Cube& cube) {
		std::string init;
		is >> init;
		cube.read_sequence(init);
		return is;
	}

	void setup(std::string const& scramble) {
		std::regex command_template("([UuDdLlRrFfBbxXyYzZeEsS])([1-9]+)?(')?");
		std::sregex_iterator rit(scramble.begin(), scramble.end(), command_template);
		std::sregex_iterator rend;
		while (rit != rend) {
			char direction = std::string((*rit)[1])[0];
			bool clockwise = (std::string((*rit)[3]).length() == 0) ? true : false;
			size_t times = (std::string((*rit)[2]).length()) ? std::stoi((*rit)[2]) : 1;
			if (direction == 'y') {
				flip('X', 'L');
			}// z->yr;   y -> xl; x->zu
			else if (direction == 'x') {
				flip('Z', 'D');
			}
			else {
				rotate(direction, clockwise, times);
			}
			//std::cout << "Performed rotation " << direction << " " << clockwise << " " << times << "\n"
			//	<< *this << "\n";
			++rit;
		}
	}
private:
	std::array<Face, 6> faces_;
	enum Faces_order {
		UP, LEFT, FRONT, RIGHT, BACK, DOWN
	};
	enum Rotation : bool {
		COUNTERCLOCKWISE, CLOCKWISE
	};
	//enum Rotation_dir {
	//	UP = 'U', DOWN = 'D', LEFT = 'L', RIGHT = 'R', BACK = 'B', FRONT = 'F'
	//};
	void horizontal_move(char direction, bool clockwise, size_t times = 1) {
		Faces_order order = (direction == 'U' || direction == 'u') ? UP : DOWN;
		times %= 4;
		for (size_t i = 0; i < times; ++i) {
			faces_[order].rotate(clockwise);
		}
		std::array<Face::Row, 4> layer = { faces_[LEFT][order % 3], faces_[FRONT][order % 3], faces_[RIGHT][order % 3], faces_[BACK][order % 3] };
		times %= 4;
		if (order == UP && clockwise || order == DOWN && !clockwise) {
			std::rotate(layer.begin(), layer.begin() + times, layer.end());
		}
		else {
			std::rotate(layer.begin(), layer.begin() + (4 - times), layer.end());
		}
		for (size_t i = LEFT; i <= BACK; ++i) {
			faces_[i][order % 3] = layer[i - 1];
		}

	}

	void side_move(char direction, bool clockwise, size_t times) {
		char anti_direction = (direction == 'L' || direction == 'l') ? 'R' : 'L';
		flip('Y', anti_direction);
		horizontal_move('U', clockwise, times);
		flip('Y', direction);
	}

	void facing_move(char direction, bool clockwise, size_t times) {
		char flip_direction = (direction == 'F' || direction == 'f') ? 'U' : 'D';
		char reverse_flip_direction = (flip_direction == 'U') ? 'D' : 'U';
		flip('Z', flip_direction);
		horizontal_move('U', clockwise, times);
		flip('Z', reverse_flip_direction);
	}
	void read_sequence(std::string const& sequence) {
		size_t stri = 0;;
		for (size_t i = 0; i < 6; ++i) {
			for (size_t j = 0; j < 3; ++j) {
				for (size_t k = 0; k < 3; ++k) {
					faces_[i][j][k] = sequence[stri];
					++stri;
				}
			}
		}
	}
	bool is_valid() const {
		return true;
	}
	void default_init() {
		using C = Cubie::Colours;
		faces_[UP] = Face({ Face::Row{C::WHITE, C::WHITE, C::WHITE}, Face::Row{C::WHITE, C::WHITE, C::WHITE}, \
		Face::Row{C::WHITE, C::WHITE, C::WHITE} }, Face::Type::UP);
		faces_[DOWN] = Face({ Face::Row{C::YELLOW, C::YELLOW, C::YELLOW}, Face::Row{C::YELLOW, C::YELLOW, C::YELLOW}, \
		Face::Row{C::YELLOW, C::YELLOW, C::YELLOW} }, Face::Type::DOWN);
		faces_[LEFT] = Face({ Face::Row{C::GREEN, C::GREEN, C::GREEN}, Face::Row{C::GREEN, C::GREEN, C::GREEN}, \
		Face::Row{C::GREEN, C::GREEN, C::GREEN } }, Face::Type::LEFT);
		faces_[RIGHT] = Face({ Face::Row{C::BLUE, C::BLUE, C::BLUE}, Face::Row{C::BLUE, C::BLUE, C::BLUE}, \
		Face::Row{C::BLUE, C::BLUE, C::BLUE } }, Face::Type::RIGHT);
		faces_[FRONT] = Face({ Face::Row{C::RED, C::RED, C::RED}, Face::Row{C::RED, C::RED, C::RED}, \
		Face::Row{C::RED, C::RED, C::RED } }, Face::Type::FRONT);
		faces_[BACK] = Face({ Face::Row{C::ORANGE, C::ORANGE, C::ORANGE}, Face::Row{C::ORANGE, C::ORANGE, C::ORANGE}, \
		Face::Row{C::ORANGE, C::ORANGE, C::ORANGE } }, Face::Type::BACK);
	}
	void flip_down() {
		Face new_up = faces_[BACK].inverted(true);
		new_up.set_type(Face::Type::UP);
		Face new_front = faces_[UP];
		new_front.set_type(Face::Type::FRONT);
		Face new_back = faces_[DOWN];
		new_back.set_type(Face::Type::BACK);
		Face new_down = faces_[FRONT].inverted(true);
		new_down.set_type(Face::Type::DOWN);
		faces_[RIGHT].rotate(COUNTERCLOCKWISE);
		faces_[LEFT].rotate(CLOCKWISE);
		faces_ = { new_up, faces_[LEFT], new_front, faces_[RIGHT], new_back, new_down };
	}
	void flip_left_y() {
		faces_[FRONT].rotate(COUNTERCLOCKWISE);
		faces_[BACK].rotate(CLOCKWISE);
		Face new_left = faces_[UP].inverted(false, false);
		new_left.set_type(Face::Type::LEFT);
		Face new_up = faces_[RIGHT].inverted(false, false);
		new_up.set_type(Face::Type::UP);
		Face new_down = faces_[LEFT].inverted(false, true);
		new_down.set_type(Face::Type::DOWN);
		Face new_right = faces_[DOWN].inverted(false, true);
		new_right.set_type(Face::Type::RIGHT);
		faces_ = { new_up, new_left, faces_[FRONT], new_right, faces_[BACK], new_down };
	}
	void flip_left_x() {
		faces_[FRONT].set_type(Face::Type::LEFT);
		faces_[LEFT].set_type(Face::Type::BACK);
		faces_[BACK].set_type(Face::Type::RIGHT);
		faces_[RIGHT].set_type(Face::Type::FRONT);
		faces_[UP] = faces_[UP].inverted(false, true);
		faces_[DOWN] = faces_[DOWN].inverted(false, false);
		faces_ = { faces_[UP], faces_[FRONT], faces_[RIGHT], faces_[BACK], faces_[LEFT], faces_[DOWN] };
	}
	//void rotate(char direction, bool clockwise = true) {
	//	switch (direction) {
	//	case 'U':
	//		if (clockwise) {
	//			std::vector<Cubie> saver = faces_[1].cubies[0];
	//			for (size_t i = 1; i < 4; ++i) {
	//				faces[i].cubies[0] = faces[i + 1].cubies[0];
	//			}
	//			faces[4].cubies[0] = saver;
	//		}
	//		else {

	//		}
	//		faces_[0].rotate(clockwise);
	//		break;
	//	case 'D':

	//		break;
	//	}
	//}
};

class CubeGenerator {

private:
	Cube cube_;
};




#endif
