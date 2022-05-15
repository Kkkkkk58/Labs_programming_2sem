#ifndef CUBIE_HPP
#define CUBIE_HPP
#include <vector>
#include "Colour.hpp"
#include "Position.hpp"
#include "Moves.hpp"

class Cubie {
public:
	virtual ~Cubie() {}
	virtual void rotate(Move const&) = 0;
};

class CenterCubie : public Cubie {
public:
	enum Faces : uint8_t {
		UP, LEFT, FRONT, RIGHT, BACK, DOWN
	};
	explicit CenterCubie(std::vector<Colour> const& colours = {C::WHITE, C::ORANGE, C::GREEN, C::RED, C::BLUE, C::YELLOW})
		: Cubie(), colours_(colours) {}
	CenterCubie(CenterCubie const& other) : colours_(other.colours_) {}
	void swap(CenterCubie& other) {
		using std::swap;
		swap(colours_, other.colours_);
	}
	CenterCubie(CenterCubie&& other) noexcept {
		swap(other);
	}
	CenterCubie& operator=(CenterCubie const& other) {
		if (this != &other) {
			colours_ = other.colours_;
		}
		return *this;
	}
	CenterCubie& operator=(CenterCubie&& other) noexcept {
		swap(other);
		return *this;
	}
	Colour& operator[](Faces const& i) {
		return colours_[i];
	}
	Colour const& operator[](Faces const& i) const {
		return colours_[i];
	}
	std::vector<Colour> const& get_colours() const {
		return colours_;
	}
	std::vector<Colour>& get_colours() {
		return colours_;
	}
	void rotate(Move const& move) override {
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
			rotate_y(move.times(), !move.clockwise());
			break;
		case Move::Direction::M:
			rotate_z(move.times(), move.clockwise());
			break;
		default: break;
		}
	}
private:
	std::vector<Colour> colours_;
	void rotate_x(uint8_t times, bool clockwise) {
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
	void rotate_y(uint8_t times, bool clockwise) {
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
	void rotate_z(uint8_t times, bool clockwise) {
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
	using C = Colour::Colours;
};

class EdgeCubie : public Cubie {
public:
	explicit EdgeCubie(EdgePosition const& orientation = EdgePosition(Position::Positions::UP, Position::Positions::FRONT))
		: Cubie(), orientation_(orientation) {}
	EdgeCubie(EdgeCubie const& other) : orientation_(other.orientation_) {}
	void swap(EdgeCubie& other) {
		using std::swap;
		swap(orientation_, other.orientation_);
	}
	EdgeCubie(EdgeCubie&& other) noexcept {
		swap(other);
	}
	EdgeCubie& operator=(EdgeCubie const& other) {
		if (this != &other) {
			orientation_ = other.orientation_;
		}
		return *this;
	}
	EdgeCubie& operator=(EdgeCubie&& other) noexcept {
		swap(other);
		return *this;
	}
	EdgePosition const& get_orientation() const {
		return orientation_;
	}
	void rotate(Move const& move) override {
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
private:
	EdgePosition orientation_;
};
 
class CornerCubie : public Cubie {
public:

	explicit CornerCubie(CornerPosition const& orientation = CornerPosition(Position::Positions::UP, \
		Position::Positions::FRONT, Position::Positions::LEFT))
		: Cubie(), orientation_(orientation) {}
	CornerCubie(CornerCubie const& other) : orientation_(other.orientation_) {}
	void swap(CornerCubie& other) {
		using std::swap;
		swap(orientation_, other.orientation_);
	}
	CornerCubie(CornerCubie&& other) noexcept {
		swap(other);
	}
	CornerCubie& operator=(CornerCubie const& other) {
		if (this != &other) {
			orientation_ = other.orientation_;
		}
		return *this;
	}
	CornerCubie& operator=(CornerCubie&& other) noexcept {
		swap(other);
		return *this;
	}
	CornerPosition const& get_orientation() const {
		return orientation_;
	}
	void rotate(Move const& move) {
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

private:
	CornerPosition orientation_;
};

#endif
