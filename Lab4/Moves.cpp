#include "Moves.hpp"
#include <regex>
#include "CubeExceptions.hpp"

// Конструктор класса Move от составляющих частей нотации поворота
Move::Move(Direction const& direction, bool clockwise, uint8_t times)
	: direction_(direction), clockwise_(clockwise), times_(times) {}

// Конструктор класса Move от строки, кодирующей поворот
Move::Move(std::string const& s) : direction_(), clockwise_(true), times_(1) {
	// Если на вход поступила пустая строка
	if (s.size() == 0) {
		throw UnknownMove("Empty move code passed");
	}
	// Получаем код поворачиваемой(-ых) грани(-ей)
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
	// Представлено другое значение
	default: throw UnknownMove(s);
	}
	// Если в строке содержится три символа
	if (s.size() == 3) {

		times_ = s[1] - '0';
		clockwise_ = (s[2] == '\'') ? false : true;
		// Если встретились ошибки в нотации
		if (times_ != 2 || clockwise_) {
			throw UnknownMove(s);
		}

	}
	// Строка потенциально содержит цифру 2 или символ '
	if (s.size() > 1) {
		switch (s[1]) {
		case '\'':
			clockwise_ = false; 
			break;
		case '2':
			times_ = 2;
			break;
		// Несоответствие нотации
		default:
			throw UnknownMove(s);
		}
	}
}

// Перевод кода движения в строковый формат
std::string Move::to_string() const {
	std::string s;
	// Задаем поворачиваемую грань
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
	// Если поворотов - два
	switch (times_) {
	case 2: s += '2'; break;
	default: break;
	}
	// Если двигаемся против часовой стрелки
	switch (clockwise_) {
	case false: s += '\''; break;
	default: break;
	}
	return s;
}

// Метод доступа к полю direction_ класса Move
Move::Direction const& Move::direction() const { return direction_; }

// Метод доступа к полю clockwise_ класса Move
bool Move::clockwise() const { return clockwise_; }

// Метод доступа к полю times_ класса Move
uint8_t Move::times() const { return times_; }

// Оператор ввода кода поворота из потока
std::istream& operator>>(std::istream& is, Move& m) {
	std::string input;
	is >> input;
	m = Move(input);
	return is;
}

// Оператор вывода кода поворота в поток
std::ostream& operator<<(std::ostream& os, Move const& m) {
	os << m.to_string();
	return os;
}

// Конструктор класса MoveSequence от вектора элементов класса Move
MoveSequence::MoveSequence(std::vector<Move> const& moves) : moves_(moves) {}

// Конструктор класса MoveSequence от строки, задающей последовательность поворотов
MoveSequence::MoveSequence(std::string const& scramble) : moves_() {
	try {
		// Регулярное выражение с возможными кодами из нотации
		std::regex command_template("([UuDdLlRrFfBbxXyYzZeEsSmM])([12])?(')?");
		std::sregex_iterator rit(scramble.begin(), scramble.end(), command_template);
		std::sregex_iterator rend;
		// Собираем последовательность поворотов
		while (rit != rend) {
			moves_.push_back(Move(std::string((*rit)[0])));
			++rit;
		}
	}
	// Обработка ошибок при обработке последовательности
	catch (std::exception const& e) {
		std::cerr << "Error while parsing scramble: " << e.what();
		moves_.clear();
	}
}

// Конструктор копирования класса MoveSequence
MoveSequence::MoveSequence(MoveSequence const& other) : moves_(other.moves_) {}

// Конструктор перемещения класса MoveSequence
MoveSequence::MoveSequence(MoveSequence&& other) noexcept {
	swap(other);
}

// Метод swap класса MoveSequence
void MoveSequence::swap(MoveSequence& other) {
	using std::swap;
	swap(moves_, other.moves_);
}

// Оператор присваивания класса MoveSequence
MoveSequence& MoveSequence::operator=(MoveSequence const& other) {
	if (this != &other) {
		moves_ = other.moves_;
	}
	return *this;
}

// Перемещающий оператор присваивания класса MoveSequence
MoveSequence& MoveSequence::operator=(MoveSequence&& other) noexcept {
	swap(other);
	return *this;
}

// Метод доступа к полю moves_ класса MoveSequence
std::vector<Move> const& MoveSequence::get_scramble() const {
	return moves_;
}

// Метод доступа к полю moves_ класса MoveSequence
std::vector<Move>& MoveSequence::get_scramble() {
	return moves_;
}

// Метод доступа к полю moves_ класса MoveSequence
Move const& MoveSequence::operator[](size_t i) const {
	return moves_[i];
}

// Метод доступа к полю moves_ класса MoveSequence
Move& MoveSequence::operator[](size_t i) {
	return moves_[i];
}

// Перевод последовательности команд в строковый формат
std::string MoveSequence::to_string() const {
	std::string s;
	for (auto const& move : moves_) {
		s += move.to_string() + " ";
	}
	return s;
}

// Получение количества поворотов в последовательности
size_t MoveSequence::size() const {
	return moves_.size();
}

// Оператор += класса MoveSequence для сложения с другой последовательностью
MoveSequence& MoveSequence::operator+=(MoveSequence const& rhs) {
	moves_.insert(moves_.end(), rhs.moves_.begin(), rhs.moves_.end());
	return *this;
}

// Оператор += класса MoveSequence для добавления одного поворота
MoveSequence& MoveSequence::operator+=(Move const& rhs) {
	moves_.push_back(rhs);
	return *this;
}

// Итератор begin класса MoveSequence
MoveSequence::iterator MoveSequence::begin() {
	return moves_.begin();
}
// Итератор begin класса MoveSequence
MoveSequence::const_iterator MoveSequence::begin() const {
	return moves_.begin();
}
// Итератор cbegin класса MoveSequence
MoveSequence::const_iterator MoveSequence::cbegin() const {
	return moves_.cbegin();
}
// Итератор end класса MoveSequence
MoveSequence::iterator MoveSequence::end() {
	return moves_.end();
}
// Итератор end класса MoveSequence
MoveSequence::const_iterator MoveSequence::end() const {
	return moves_.end();
}
// Итератор cend класса MoveSequence
MoveSequence::const_iterator MoveSequence::cend() const {
	return moves_.cend();
}
// Итератор rbegin класса MoveSequence
MoveSequence::reverse_iterator MoveSequence::rbegin() {
	return moves_.rbegin();
}
// Итератор rbegin класса MoveSequence
MoveSequence::const_reverse_iterator MoveSequence::rbegin() const {
	return moves_.rbegin();
}
// Итератор crbegin класса MoveSequence
MoveSequence::const_reverse_iterator MoveSequence::crbegin() const {
	return moves_.crbegin();
}
// Итератор rend класса MoveSequence
MoveSequence::reverse_iterator MoveSequence::rend() {
	return moves_.rend();
}
// Итератор rend класса MoveSequence
MoveSequence::const_reverse_iterator MoveSequence::rend() const {
	return moves_.rend();
}
// Итератор crend класса MoveSequence
MoveSequence::const_reverse_iterator MoveSequence::crend() const {
	return moves_.crend();
}

// Сложение двух последовательностей
const MoveSequence operator+(MoveSequence lhs, MoveSequence const& rhs) {
	return lhs += rhs;
}

// Сложение последовательности с отдельным поворотом
const MoveSequence operator+(MoveSequence lhs, Move const& rhs) {
	return lhs += rhs;
}

// Сложение последовательности с отдельным поворотом
const MoveSequence operator+(Move const& lhs, MoveSequence rhs) {
	return rhs += lhs;
}

// Оператор вывода в поток последовательности поворотов
std::ostream& operator<<(std::ostream& os, MoveSequence const& m) {
	os << m.to_string();
	return os;
}

// Оператор ввода из потока последовательности поворотов
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
