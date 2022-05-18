#ifndef MOVES_HPP
#define MOVES_HPP
#include <cinttypes>
#include <string>
#include <regex>
#include "CubeExceptions.hpp"

class Move {
public:
	enum class Direction : char {
		U, D, L, R, F, B, X, Y, Z, M, E, S
	};
	Move(Direction const& direction, bool clockwise, uint8_t times)
		: direction_(direction), clockwise_(clockwise), times_(times) {}
	Move(std::string const& s) : direction_(), clockwise_(true), times_(1) {
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


class MoveSequence {
public:
	using iterator = std::vector<Move>::iterator;
	using const_iterator = std::vector<Move>::const_iterator;
	using reverse_iterator = std::vector<Move>::reverse_iterator;
	using const_reverse_iterator = std::vector<Move>::const_reverse_iterator;

	explicit MoveSequence(std::vector<Move> const& moves) : moves_(moves) {}
	MoveSequence(std::string const& scramble) : moves_() {
		try {
			std::regex command_template("([UuDdLlRrFfBbxXyYzZeEsSmM])([12])?(')?");
			std::sregex_iterator rit(scramble.begin(), scramble.end(), command_template);
			std::sregex_iterator rend;
			while (rit != rend) {
				moves_.push_back(Move(std::string((*rit)[0])));
				++rit;
			}
		}
		catch (std::exception const& e) {
			std::cerr << "Error while parsing scramble: " << e.what();
			moves_.clear();
		}
	}
	MoveSequence(MoveSequence const& other) : moves_(other.moves_) {}
	void swap(MoveSequence& other) {
		using std::swap;
		swap(moves_, other.moves_);
	}
	MoveSequence(MoveSequence&& other) noexcept {
		swap(other);
	}
	MoveSequence& operator=(MoveSequence const& other) {
		if (this != &other) {
			moves_ = other.moves_;
		}
	}
	MoveSequence& operator=(MoveSequence&& other) noexcept {
		swap(other);
		return *this;
	}
	~MoveSequence() = default;
	std::vector<Move> const& get_scramble() const {
		return moves_;
	}
	std::vector<Move>& get_scramble() {
		return moves_;
	}
	Move const& operator[](size_t i) const {
		return moves_[i];
	}
	Move& operator[](size_t i) {
		return moves_[i];
	}
	std::string to_string() const {
		std::string s;
		for (auto const& move : moves_) {
			s += move.to_string();
		}
		return s;
	}
	size_t size() const {
		return moves_.size();
	}

	iterator begin() {
		return moves_.begin();
	}
	const_iterator begin() const {
		return moves_.begin();
	}
	const_iterator cbegin() const {
		return moves_.cbegin();
	}
	iterator end() {
		return moves_.end();
	}
	const_iterator end() const {
		return moves_.end();
	}
	const_iterator cend() const {
		return moves_.cend();
	}
	reverse_iterator rbegin() {
		return moves_.rbegin();
	}
	const_reverse_iterator rbegin() const {
		return moves_.rbegin();
	}
	const_reverse_iterator crbegin() const {
		return moves_.crbegin();
	}
	reverse_iterator rend() {
		return moves_.rend();
	}
	const_reverse_iterator rend() const {
		return moves_.rend();
	}
	const_reverse_iterator crend() const {
		return moves_.crend();
	}
private:
	std::vector<Move> moves_;
};


std::ostream& operator<<(std::ostream& os, MoveSequence const& m) {
	os << m.to_string();
	return os;
}

std::istream& operator>>(std::istream& is, MoveSequence& m) {
	std::string input;
	while (is.good()) {
		std::string tmp;
		is >> tmp;
		input += tmp;
	}
	m = MoveSequence(input);
	return is;
}

#endif
