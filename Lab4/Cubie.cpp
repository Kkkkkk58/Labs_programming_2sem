#include "Cubie.hpp"

// ����������� ������ CenterCubie �� ������������������ ������
CenterCubie::CenterCubie(std::vector<Colour> const& colours)
	: Cubie(), colours_(colours) {}

// ����������� ����������� ������ CenterCubie
CenterCubie::CenterCubie(CenterCubie const& other) : colours_(other.colours_) {}

// ����� swap ������ CenterCubie
void CenterCubie::swap(CenterCubie& other) {
	using std::swap;
	swap(colours_, other.colours_);
}

// ����������� ����������� ������ CenterCubie
CenterCubie::CenterCubie(CenterCubie&& other) noexcept {
	swap(other);
}

// �������� ������������ ������ CenterCubie
CenterCubie& CenterCubie::operator=(CenterCubie const& other) {
	if (this != &other) {
		colours_ = other.colours_;
	}
	return *this;
}

// ������������ �������� ������������ ������ CenterCubie
CenterCubie& CenterCubie::operator=(CenterCubie&& other) noexcept {
	swap(other);
	return *this;
}

// ��������� ������� � ����������� ����� ������
Position::Positions CenterCubie::operator[](uint8_t i) const {
	return colours_[i];
}
// ��������� ������� � ����������� ����� ������
Colour& CenterCubie::operator[](uint8_t i) {
	return colours_[i];
}

// ��������� ���� ������
std::vector<Colour> const& CenterCubie::get_colours() const {
	return colours_;
}
// ��������� ���� ������
std::vector<Colour>& CenterCubie::get_colours() {
	return colours_;
}



// ����������� ������ EdgeCubie �� EdgePosition
EdgeCubie::EdgeCubie(EdgePosition const& orientation)
	: Cubie(), orientation_(orientation) {}

// ����������� ����������� ������ EdgeCubie
EdgeCubie::EdgeCubie(EdgeCubie const& other) : orientation_(other.orientation_) {}

// ����� swap ������ EdgeCubie
void EdgeCubie::swap(EdgeCubie& other) {
	using std::swap;
	swap(orientation_, other.orientation_);
}

// ����������� ����������� ������ EdgeCubie
EdgeCubie::EdgeCubie(EdgeCubie&& other) noexcept {
	swap(other);
}

// �������� ������������ ������ EdgeCubie
EdgeCubie& EdgeCubie::operator=(EdgeCubie const& other) {
	if (this != &other) {
		orientation_ = other.orientation_;
	}
	return *this;
};

// ������������ �������� ������������ ������ EdgeCubie
EdgeCubie& EdgeCubie::operator=(EdgeCubie&& other) noexcept {
	swap(other);
	return *this;
}

// ��������� ������� � ���� orientation_ ������ EdgeCubie
EdgePosition const& EdgeCubie::get_orientation() const {
	return orientation_;
}

// ��������� ������� � ������������ �����
Position::Positions EdgeCubie::operator[](uint8_t i) const {
	return orientation_[i];
}

// ��������� ������� ������
std::vector<Colour> EdgeCubie::get_colours() const {
	return { orientation_[0], orientation_[1] };
}

// �������� ��������� ������
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


// ����������� ������ CornerCubie �� CornerPosition
CornerCubie::CornerCubie(CornerPosition const& orientation)
	: Cubie(), orientation_(orientation) {}

// ����������� ����������� ������ CornerPosition
CornerCubie::CornerCubie(CornerCubie const& other) : orientation_(other.orientation_) {}

// ����� swap ������ CornerPosition
void CornerCubie::swap(CornerCubie& other) {
	using std::swap;
	swap(orientation_, other.orientation_);
}

// ����������� ����������� ������ CornerPosition
CornerCubie::CornerCubie(CornerCubie&& other) noexcept {
	swap(other);
}

// �������� ������������ ������ CornerPosition
CornerCubie& CornerCubie::operator=(CornerCubie const& other) {
	if (this != &other) {
		orientation_ = other.orientation_;
	}
	return *this;
}

// ������������ �������� ������������ ������ CornerPosition
CornerCubie& CornerCubie::operator=(CornerCubie&& other) noexcept {
	swap(other);
	return *this;
}

// ��������� ������� � ���� orientation_ ������ CornerPosition
CornerPosition const& CornerCubie::get_orientation() const {
	return orientation_;
}

// ��������� ����������� �����
Position::Positions CornerCubie::operator[](uint8_t i) const {
	return orientation_[i];
}

// �������� �������� ������
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

// ��������� ���� ������
std::vector<Colour> CornerCubie::get_colours() const {
	return { orientation_[0], orientation_[1], orientation_[2] };
}