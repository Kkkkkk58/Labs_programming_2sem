#ifndef MOVES_HPP
#define MOVES_HPP
#include <cinttypes>
#include <string>
#include "CubeExceptions.hpp"

class Move {
public:
	enum class Direction : char {
		U, D, L, R, F, B, X, Y, Z, M, E, S
	};
	Move(Direction const& direction, bool clockwise, uint8_t times)
		: direction_(direction), clockwise_(clockwise), times_(times) {}
	explicit Move(std::string const& s) : direction_(), clockwise_(true), times_(1) {
		if (s.size() == 0) {
			throw UnknownMove("Empty move code passed");
		}
		switch (s[0]) {
		case 'U': case 'u': direction_ = Direction::U; break;
		case 'D': case 'd': direction_ = Direction::D; break;
		case 'L': case 'l': direction_ = Direction::L; break;
		case 'R': case 'r': direction_ = Direction::R; break;
		case 'F': case 'f': direction_ = Direction::F; break;
		case 'B': case 'b': direction_ = Direction::B; break;
		case 'X': case 'x': direction_ = Direction::X; break;
		case 'Y': case 'y': direction_ = Direction::Y; break;
		case 'Z': case 'z': direction_ = Direction::Z; break;
		case 'M': case 'm': direction_ = Direction::M; break;
		case 'E': case 'e': direction_ = Direction::E; break;
		case 'S': case 's': direction_ = Direction::S; break;
		default: throw UnknownMove(s);
		}
		if (s.size() == 3) {

			times_ = s[1] - '0';
			clockwise_ = (s[2] == '\'') ? false : true;
			if (times_ != 2 || clockwise_) {
				throw UnknownMove(s);
			}
	
		}
		if (s.size() > 1) {
			switch (s[1]) {
			case '\'':
				clockwise_ = false; break;
			case '2':
				times_ = 2; break;
			default:
				throw UnknownMove(s);
			}
		}
	}
	std::string to_string() const {
		std::string s;
		switch (direction_) {
		case Direction::U: s += 'U'; break;
		case Direction::D: s += 'D'; break;
		case Direction::L: s += 'L'; break;
		case Direction::R: s += 'R'; break;
		case Direction::F: s += 'F'; break;
		case Direction::B: s += 'B'; break;
		case Direction::M: s += 'M'; break;
		case Direction::S: s += 'S'; break;
		case Direction::E: s += 'E'; break;
		case Direction::X: s += 'X'; break;
		case Direction::Y: s += 'Y'; break;
		case Direction::Z: s += 'Z'; break;
		}
		switch (times_) {
		case 2: s += '2'; break;
		default: break;
		}
		switch (clockwise_) {
		case false: s += '\''; break;
		default: break;
		}
		return s;
	}
	Direction const& direction() const {
		return direction_;
	}
	bool clockwise() const {
		return clockwise_;
	}
	uint8_t times() const {
		return times_;
	}
private:
	Direction direction_;
	bool clockwise_;
	uint8_t times_;
};


std::istream& operator>>(std::istream& is, Move& m) {
	std::string input;
	is >> input;
	m = Move(input);
	return is;
}

std::ostream& operator<<(std::ostream& os, Move const& m) {
	os << m.to_string();
	return os;
}

#endif
