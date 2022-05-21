#ifndef MOVES_HPP
#define MOVES_HPP
#include <cinttypes>
#include <string>
#include <vector>

// Класс, предоставляющий информацию об одном повороте
class Move {
public:
	// Коды для поворотов граней из стандартной нотации
	enum class Direction : char {
		U, D, L, R, F, B, X, Y, Z, M, E, S
	};
	Move(Direction const&, bool, uint8_t);
	Move(std::string const&);
	std::string to_string() const;
	Direction const& direction() const;
	bool clockwise() const;
	uint8_t times() const;
private:
	Direction direction_;	// Код поворачиваемой(-ых) грани(-ей)
	bool clockwise_;		// Направление поворота - true == по часовой стрелке, false == против часовой
	uint8_t times_;			// Количество раз (в стандартной нотации от 1 до 2)
};


std::istream& operator>>(std::istream&, Move&);

std::ostream& operator<<(std::ostream& , Move const&);

// Контейнер - последовательность поворотов
class MoveSequence {
public:
	using iterator				 = std::vector<Move>::iterator;
	using const_iterator		 = std::vector<Move>::const_iterator;
	using reverse_iterator		 = std::vector<Move>::reverse_iterator;
	using const_reverse_iterator = std::vector<Move>::const_reverse_iterator;

	explicit MoveSequence(std::vector<Move> const& = std::vector<Move>());
	MoveSequence(std::string const&);
	MoveSequence(MoveSequence const&);
	void swap(MoveSequence&);
	MoveSequence(MoveSequence&&) noexcept;
	MoveSequence& operator=(MoveSequence const&);
	MoveSequence& operator=(MoveSequence&&) noexcept;
	~MoveSequence() = default;
	std::vector<Move> const& get_scramble() const;
	std::vector<Move>& get_scramble();
	Move const& operator[](size_t) const;
	Move& operator[](size_t);
	std::string to_string() const;
	size_t size() const;
	MoveSequence& operator+=(MoveSequence const&);
	MoveSequence& operator+=(Move const&);

	iterator begin();
	const_iterator begin() const;
	const_iterator cbegin() const;
	iterator end();
	const_iterator end() const;
	const_iterator cend() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	const_reverse_iterator crbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;
	const_reverse_iterator crend() const;
private:
	std::vector<Move> moves_;		// Вектор поворотов
};

const MoveSequence operator+(MoveSequence, MoveSequence const&);

const MoveSequence operator+(MoveSequence, Move const&);

const MoveSequence operator+(Move const&, MoveSequence);

std::ostream& operator<<(std::ostream&, MoveSequence const&);

std::istream& operator>>(std::istream&, MoveSequence&);

#endif
