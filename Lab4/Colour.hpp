#ifndef COLOUR_HPP
#define COLOUR_HPP
#include <cinttypes>
#include <iostream>
#include "Position.hpp"

// Класс, определяющий цвета - для индексации и взаимодействия с внешним миром
class Colour {
public:
	enum class Colours : uint8_t {
		WHITE, BLUE, RED, GREEN, YELLOW, ORANGE
	};
	Colour(Colours = Colours::WHITE);
	Colour(Position::Positions const&);
	Colour(char);
	~Colour() = default;
	Colour(Colour const&);
	void swap(Colour&);
	Colour(Colour&&) noexcept;
	Colour& operator=(Colour const&);
	Colour& operator=(Colour&&) noexcept;
	bool operator==(Colour const&) const;
	bool operator!=(Colour const&) const;
	operator Position::Positions() const;
	operator char() const;
private:
	Colours colour_;	// Объект типа перечисления
};

uint8_t index(Colour const&);

std::ostream& operator<<(std::ostream& os, Position::Positions const&);

std::istream& operator>>(std::istream&, Colour&);

#endif
