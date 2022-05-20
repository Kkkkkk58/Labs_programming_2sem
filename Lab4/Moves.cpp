#include "Moves.hpp"
#include <regex>
#include "CubeExceptions.hpp"


Move::Move(Direction const& direction, bool clockwise, uint8_t times)
	: direction_(direction), clockwise_(clockwise), times_(times) {}

Move::Move(std::string const& s) : direction_(), clockwise_(true), times_(1) {
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

std::string Move::to_string() const {
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

Move::Direction const& Move::direction() const { return direction_; }
bool Move::clockwise() const { return clockwise_; }
uint8_t Move::times() const { return times_; }


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


MoveSequence::MoveSequence(std::vector<Move> const& moves) : moves_(moves) {}

MoveSequence::MoveSequence(std::string const& scramble) : moves_() {
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

MoveSequence::MoveSequence(MoveSequence const& other) : moves_(other.moves_) {}

MoveSequence::MoveSequence(MoveSequence&& other) noexcept {
	swap(other);
}

void MoveSequence::swap(MoveSequence& other) {
	using std::swap;
	swap(moves_, other.moves_);
}

MoveSequence& MoveSequence::operator=(MoveSequence const& other) {
	if (this != &other) {
		moves_ = other.moves_;
	}
	return *this;
}

MoveSequence& MoveSequence::operator=(MoveSequence&& other) noexcept {
	swap(other);
	return *this;
}

std::vector<Move> const& MoveSequence::get_scramble() const {
	return moves_;
}

std::vector<Move>& MoveSequence::get_scramble() {
	return moves_;
}

Move const& MoveSequence::operator[](size_t i) const {
	return moves_[i];
}

Move& MoveSequence::operator[](size_t i) {
	return moves_[i];
}

std::string MoveSequence::to_string() const {
	std::string s;
	for (auto const& move : moves_) {
		s += move.to_string() + " ";
	}
	return s;
}
size_t MoveSequence::size() const {
	return moves_.size();
}

MoveSequence& MoveSequence::operator+=(MoveSequence const& rhs) {
	moves_.insert(moves_.end(), rhs.moves_.begin(), rhs.moves_.end());
	return *this;
}


MoveSequence& MoveSequence::operator+=(Move const& rhs) {
	moves_.push_back(rhs);
	return *this;
}


MoveSequence::iterator MoveSequence::begin() {
	return moves_.begin();
}
MoveSequence::const_iterator MoveSequence::begin() const {
	return moves_.begin();
}
MoveSequence::const_iterator MoveSequence::cbegin() const {
	return moves_.cbegin();
}
MoveSequence::iterator MoveSequence::end() {
	return moves_.end();
}
MoveSequence::const_iterator MoveSequence::end() const {
	return moves_.end();
}
MoveSequence::const_iterator MoveSequence::cend() const {
	return moves_.cend();
}
MoveSequence::reverse_iterator MoveSequence::rbegin() {
	return moves_.rbegin();
}
MoveSequence::const_reverse_iterator MoveSequence::rbegin() const {
	return moves_.rbegin();
}
MoveSequence::const_reverse_iterator MoveSequence::crbegin() const {
	return moves_.crbegin();
}
MoveSequence::reverse_iterator MoveSequence::rend() {
	return moves_.rend();
}
MoveSequence::const_reverse_iterator MoveSequence::rend() const {
	return moves_.rend();
}
MoveSequence::const_reverse_iterator MoveSequence::crend() const {
	return moves_.crend();
}


const MoveSequence operator+(MoveSequence lhs, MoveSequence const& rhs) {
	return lhs += rhs;
}

const MoveSequence operator+(MoveSequence lhs, Move const& rhs) {
	return lhs += rhs;
}

const MoveSequence operator+(Move const& lhs, MoveSequence rhs) {
	return rhs += lhs;
}

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
