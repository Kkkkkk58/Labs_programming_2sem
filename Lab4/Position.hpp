#ifndef POSITION_HPP
#define POSITION_HPP
#include <iostream>
#include <cinttypes>
#include <bit>
#include <bitset>
#include "Constants.hpp"

// Интерфейс для классов, описывающих внутреннее устройство маленьких кубиков
class Position {
public:
	// Внутреннее представление цвета - степенями двойки -> взаимно не перескаются -> можно использовать побитовые операции
	enum class Positions : uint8_t {
		WHITE = 0b1, BLUE = 0b10, RED = 0b100, GREEN = 0b1000, YELLOW = 0b10000, ORANGE = 0b100000
	};
	virtual Positions operator[](uint8_t) const = 0;
	virtual void swap_bits(uint8_t, uint8_t) = 0;
	virtual ~Position() {}
};

// Позиция углового кубика
class CornerPosition : public Position {
public:
	CornerPosition();
	CornerPosition(Positions const&, Positions const&, Positions const&);
	CornerPosition(CornerPosition const&);
	void swap(CornerPosition&);
	CornerPosition(CornerPosition&&) noexcept;
	CornerPosition& operator=(CornerPosition const&);
	CornerPosition& operator=(CornerPosition&&) noexcept;
	void swap_bits(uint8_t, uint8_t) override;
	Positions operator[](uint8_t i) const override;
private:
	std::bitset<COLOURS_COUNT * 3> bits_;		// Битсет, содержащий три цвета в порядке U/D -> F/B -> L/R
};


// Позиция реберного кубика
class EdgePosition : public Position {
public:
	EdgePosition();
	EdgePosition(Positions const&, Positions const&);
	EdgePosition(EdgePosition const&);
	void swap(EdgePosition&);
	EdgePosition(EdgePosition&&) noexcept;
	EdgePosition& operator=(EdgePosition const&);
	EdgePosition& operator=(EdgePosition&&) noexcept;
	void swap_bits(uint8_t, uint8_t) override;
	Positions operator[](uint8_t i) const override;
private:
	std::bitset<COLOURS_COUNT * 2> bits_;	// Битсет, содержащий цвета в порядке U/D -> L/R/F/B - для верхнего и нижнего слоя
											// и в порядке F/B -> L/R для среднего слоя
};



#endif