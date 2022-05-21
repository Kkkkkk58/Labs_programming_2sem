#include "Cubie.hpp"

// Конструктор класса CenterCubie от последовательности цветов
CenterCubie::CenterCubie(std::vector<Colour> const& colours)
	: Cubie(), colours_(colours) {}

// Конструктор копирования класса CenterCubie
CenterCubie::CenterCubie(CenterCubie const& other) : colours_(other.colours_) {}

// Метод swap класса CenterCubie
void CenterCubie::swap(CenterCubie& other) {
	using std::swap;
	swap(colours_, other.colours_);
}

// Конструктор перемещения класса CenterCubie
CenterCubie::CenterCubie(CenterCubie&& other) noexcept {
	swap(other);
}

// Оператор присваивания класса CenterCubie
CenterCubie& CenterCubie::operator=(CenterCubie const& other) {
	if (this != &other) {
		colours_ = other.colours_;
	}
	return *this;
}

// Перемещающий оператор присваивания класса CenterCubie
CenterCubie& CenterCubie::operator=(CenterCubie&& other) noexcept {
	swap(other);
	return *this;
}

// Получение доступа к конкретному цвету кубика
Position::Positions CenterCubie::operator[](uint8_t i) const {
	return colours_[i];
}
// Получение доступа к конкретному цвету кубика
Colour& CenterCubie::operator[](uint8_t i) {
	return colours_[i];
}

// Получение всех цветов
std::vector<Colour> const& CenterCubie::get_colours() const {
	return colours_;
}
// Получение всех цветов
std::vector<Colour>& CenterCubie::get_colours() {
	return colours_;
}



// Конструктор класса EdgeCubie от EdgePosition
EdgeCubie::EdgeCubie(EdgePosition const& orientation)
	: Cubie(), orientation_(orientation) {}

// Конструктор копирования класса EdgeCubie
EdgeCubie::EdgeCubie(EdgeCubie const& other) : orientation_(other.orientation_) {}

// Метод swap класса EdgeCubie
void EdgeCubie::swap(EdgeCubie& other) {
	using std::swap;
	swap(orientation_, other.orientation_);
}

// Конструктор перемещения класса EdgeCubie
EdgeCubie::EdgeCubie(EdgeCubie&& other) noexcept {
	swap(other);
}

// Оператор присваивания класса EdgeCubie
EdgeCubie& EdgeCubie::operator=(EdgeCubie const& other) {
	if (this != &other) {
		orientation_ = other.orientation_;
	}
	return *this;
};

// Перемещающий оператор присваивания класса EdgeCubie
EdgeCubie& EdgeCubie::operator=(EdgeCubie&& other) noexcept {
	swap(other);
	return *this;
}

// Получение доступа к полю orientation_ класса EdgeCubie
EdgePosition const& EdgeCubie::get_orientation() const {
	return orientation_;
}

// Получение доступа к определнному цвету
Position::Positions EdgeCubie::operator[](uint8_t i) const {
	return orientation_[i];
}

// Получение вектора цветов
std::vector<Colour> EdgeCubie::get_colours() const {
	return { orientation_[0], orientation_[1] };
}

// Вращение реберного кубика
void EdgeCubie::rotate(Move const& move) {
	if (move.times() != 2) {
		switch (move.direction()) {
		case Move::Direction::F: case Move::Direction::B:
		case Move::Direction::X: case Move::Direction::Y: case Move::Direction::Z:
		case Move::Direction::M: case Move::Direction::E: case Move::Direction::S:
			orientation_.swap_bits(0, 1);
			break;
		default: break;
		}
	}
}


// Конструктор класса CornerCubie от CornerPosition
CornerCubie::CornerCubie(CornerPosition const& orientation)
	: Cubie(), orientation_(orientation) {}

// Конструктор копирования класса CornerPosition
CornerCubie::CornerCubie(CornerCubie const& other) : orientation_(other.orientation_) {}

// Метод swap класса CornerPosition
void CornerCubie::swap(CornerCubie& other) {
	using std::swap;
	swap(orientation_, other.orientation_);
}

// Конструктор перемещения класса CornerPosition
CornerCubie::CornerCubie(CornerCubie&& other) noexcept {
	swap(other);
}

// Оператор присваивания класса CornerPosition
CornerCubie& CornerCubie::operator=(CornerCubie const& other) {
	if (this != &other) {
		orientation_ = other.orientation_;
	}
	return *this;
}

// Перемещающий оператор присваивания класса CornerPosition
CornerCubie& CornerCubie::operator=(CornerCubie&& other) noexcept {
	swap(other);
	return *this;
}

// Получение доступа к полю orientation_ класса CornerPosition
CornerPosition const& CornerCubie::get_orientation() const {
	return orientation_;
}

// Получение конкретного цвета
Position::Positions CornerCubie::operator[](uint8_t i) const {
	return orientation_[i];
}

// Вращение углового кубика
void CornerCubie::rotate(Move const& move) {
	if (move.times() != 2) {
		switch (move.direction()) {
		case Move::Direction::U: case Move::Direction::D: case Move::Direction::Y:
			orientation_.swap_bits(1, 2);
			break;
		case Move::Direction::R: case Move::Direction::L: case Move::Direction::X:
			orientation_.swap_bits(0, 1);
			break;
		case Move::Direction::F: case Move::Direction::B: case Move::Direction::Z:
			orientation_.swap_bits(0, 2);
			break;
		default: break;
		}
	}
}

// Получение всех цветов
std::vector<Colour> CornerCubie::get_colours() const {
	return { orientation_[0], orientation_[1], orientation_[2] };
}