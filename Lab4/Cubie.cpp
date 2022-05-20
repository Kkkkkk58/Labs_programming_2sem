#include "Cubie.hpp"

CenterCubie::CenterCubie(std::vector<Colour> const& colours)
	: Cubie(), colours_(colours) {}

CenterCubie::CenterCubie(CenterCubie const& other) : colours_(other.colours_) {}

void CenterCubie::swap(CenterCubie& other) {
	using std::swap;
	swap(colours_, other.colours_);
}

CenterCubie::CenterCubie(CenterCubie&& other) noexcept {
	swap(other);
}

CenterCubie& CenterCubie::operator=(CenterCubie const& other) {
	if (this != &other) {
		colours_ = other.colours_;
	}
	return *this;
}

CenterCubie& CenterCubie::operator=(CenterCubie&& other) noexcept {
	swap(other);
	return *this;
}

Position::Positions CenterCubie::operator[](uint8_t i) const {
	return colours_[i];
}
Colour& CenterCubie::operator[](uint8_t i) {
	return colours_[i];
}

std::vector<Colour> const& CenterCubie::get_colours() const {
	return colours_;
}
std::vector<Colour>& CenterCubie::get_colours() {
	return colours_;
}

void CenterCubie::rotate(Move const& move) {
	switch (move.direction()) {
	case Move::Direction::X:
		rotate_x(move.times(), move.clockwise());
		break;
	case Move::Direction::Y:
		rotate_y(move.times(), move.clockwise());
		break;
	case Move::Direction::Z:
		rotate_z(move.times(), move.clockwise());
		break;
	case Move::Direction::E:
		rotate_y(move.times(), move.clockwise());
		break;
	case Move::Direction::M:
		rotate_x(move.times(), !move.clockwise());
		break;
	case Move::Direction::S:
		rotate_z(move.times(), move.clockwise());
		break;
	default: break;
	}
}

void CenterCubie::rotate_x(uint8_t times, bool clockwise) {
	if (times == 2) {
		std::swap(colours_[Faces::UP], colours_[Faces::DOWN]);
		std::swap(colours_[Faces::FRONT], colours_[Faces::BACK]);
	}
	else {
		if (clockwise) {
			colours_ = { colours_[Faces::FRONT], colours_[Faces::LEFT], colours_[Faces::DOWN],
						colours_[Faces::RIGHT], colours_[Faces::UP], colours_[Faces::BACK] };
		}
		else {
			colours_ = { colours_[Faces::BACK], colours_[Faces::LEFT], colours_[Faces::UP],
						colours_[Faces::RIGHT], colours_[Faces::DOWN], colours_[Faces::FRONT] };
		}
	}
}

void CenterCubie::rotate_y(uint8_t times, bool clockwise) {
	if (times == 2) {
		std::swap(colours_[Faces::LEFT], colours_[Faces::RIGHT]);
		std::swap(colours_[Faces::FRONT], colours_[Faces::BACK]);
	}
	else {
		if (clockwise) {
			std::rotate(colours_.begin() + Faces::LEFT, colours_.begin() + Faces::BACK, colours_.begin() + Faces::DOWN);
		}
		else {
			std::rotate(colours_.begin() + Faces::LEFT, colours_.begin() + Faces::FRONT, colours_.begin() + Faces::DOWN);
		}
	}
}

void CenterCubie::rotate_z(uint8_t times, bool clockwise) {
	if (times == 2) {
		std::swap(colours_[Faces::UP], colours_[Faces::DOWN]);
		std::swap(colours_[Faces::LEFT], colours_[Faces::RIGHT]);
	}
	else {
		if (clockwise) {
			colours_ = { colours_[Faces::LEFT], colours_[Faces::DOWN], colours_[Faces::FRONT],
						colours_[Faces::UP], colours_[Faces::BACK], colours_[Faces::RIGHT] };
		}
		else {
			colours_ = { colours_[Faces::RIGHT], colours_[Faces::UP], colours_[Faces::FRONT],
						colours_[Faces::DOWN], colours_[Faces::BACK], colours_[Faces::LEFT] };
		}
	}
}




EdgeCubie::EdgeCubie(EdgePosition const& orientation)
	: Cubie(), orientation_(orientation) {}

EdgeCubie::EdgeCubie(EdgeCubie const& other) : orientation_(other.orientation_) {}
void EdgeCubie::swap(EdgeCubie& other) {
	using std::swap;
	swap(orientation_, other.orientation_);
}
EdgeCubie::EdgeCubie(EdgeCubie&& other) noexcept {
	swap(other);
}

EdgeCubie& EdgeCubie::operator=(EdgeCubie const& other) {
	if (this != &other) {
		orientation_ = other.orientation_;
	}
	return *this;
};

EdgeCubie& EdgeCubie::operator=(EdgeCubie&& other) noexcept {
	swap(other);
	return *this;
}

EdgePosition const& EdgeCubie::get_orientation() const {
	return orientation_;
}

Position::Positions EdgeCubie::operator[](uint8_t i) const {
	return orientation_[i];
}
std::vector<Colour> EdgeCubie::get_colours() const {
	return { orientation_[0], orientation_[1] };
}

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


CornerCubie::CornerCubie(CornerPosition const& orientation)
	: Cubie(), orientation_(orientation) {}

CornerCubie::CornerCubie(CornerCubie const& other) : orientation_(other.orientation_) {}
void CornerCubie::swap(CornerCubie& other) {
	using std::swap;
	swap(orientation_, other.orientation_);
}
CornerCubie::CornerCubie(CornerCubie&& other) noexcept {
	swap(other);
}

CornerCubie& CornerCubie::operator=(CornerCubie const& other) {
	if (this != &other) {
		orientation_ = other.orientation_;
	}
	return *this;
}

CornerCubie& CornerCubie::operator=(CornerCubie&& other) noexcept {
	swap(other);
	return *this;
}

CornerPosition const& CornerCubie::get_orientation() const {
	return orientation_;
}
Position::Positions CornerCubie::operator[](uint8_t i) const {
	return orientation_[i];
}

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

std::vector<Colour> CornerCubie::get_colours() const {
	return { orientation_[0], orientation_[1], orientation_[2] };
}