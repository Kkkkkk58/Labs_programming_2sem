#ifndef POSITION_HPP
#define POSITION_HPP
#include <iostream>
#include <cinttypes>
#include <bit>
#include <bitset>
#include "Constants.hpp"

class Position {
public:
	enum class Positions : uint8_t {
		WHITE = 0b1, BLUE = 0b10, RED = 0b100, GREEN = 0b1000, YELLOW = 0b10000, ORANGE = 0b100000
	};
	virtual Positions operator[](uint8_t) const = 0;
	virtual void swap_bits(uint8_t, uint8_t) = 0;
	virtual ~Position() {}
};


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
	std::bitset<COLOURS_COUNT * 3> bits_;
};


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
	std::bitset<COLOURS_COUNT * 2> bits_;
};



#endif