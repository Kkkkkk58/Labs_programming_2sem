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

//std::ostream& operator>>(std::ostream& os, Position::Positions const& pos) {
//	switch (pos) {
//	case Position::Positions::UP: std::cout << "U"; break;
//	case Position::Positions::LEFT: std::cout << "L"; break;
//	case Position::Positions::FRONT: std::cout << "F"; break;
//	case Position::Positions::BACK: std::cout << "B"; break;
//	case Position::Positions::RIGHT: std::cout << "R"; break;
//	case Position::Positions::DOWN: std::cout << "D"; break;
//	}
//	return os;
//}
class CornerPosition : public Position {
public:

	CornerPosition() : bits_() {}
	CornerPosition(Positions const& a, Positions const& b, Positions const& c)
		: bits_((static_cast<uint16_t>(a) << (COLOURS_COUNT * 2)) | (static_cast<uint16_t>(b) << COLOURS_COUNT) | static_cast<uint8_t>(c)) {}



	void swap_bits(uint8_t a, uint8_t b) override {
		if (a < b) {
			std::swap(a, b);
		}
		std::bitset<COLOURS_COUNT * 3> first_bit_mask((0b111111 << ((2 - a) * COLOURS_COUNT)));
		std::bitset<COLOURS_COUNT * 3> second_bit_mask((0b111111 << ((2 - b) * COLOURS_COUNT)));
		if ((a | b) == 0b10) {
			uint8_t pentabits_avail = 1;
			std::bitset<COLOURS_COUNT * 3> third_bit_mask = 0b111111 << (pentabits_avail * COLOURS_COUNT);
			bits_ = ((bits_ & first_bit_mask) >> COLOURS_COUNT * 2) | ((bits_ & second_bit_mask) << COLOURS_COUNT * 2) | (bits_ & third_bit_mask);
		}
		else {
			uint8_t pentabits_avail = ((a | b) == 0b1) ? 0 : 2;
			std::bitset<COLOURS_COUNT * 3> third_bit_mask = 0b111111 << (pentabits_avail * COLOURS_COUNT);
			bits_ = ((bits_ & first_bit_mask) << COLOURS_COUNT) | ((bits_ & second_bit_mask) >> COLOURS_COUNT) | (bits_ & third_bit_mask);
		}
	}

	Positions operator[](uint8_t i) const override {
		std::bitset<COLOURS_COUNT * 3> bit_mask(0b111111);
		//std::cout << "\n" << (((bits_ >> 12) & bit_mask).to_ulong()) << "\n";
		return static_cast<Positions>(((bits_ >> (COLOURS_COUNT * (2 - i))) & bit_mask).to_ulong());
	}
private:
	std::bitset<COLOURS_COUNT * 3> bits_;
};

class EdgePosition : public Position {
public:
	EdgePosition() : bits_() {}
	EdgePosition(Positions const& a, Positions const& b)
		: bits_((static_cast<uint16_t>(a) << COLOURS_COUNT) | static_cast<uint8_t>(b)) {}
	EdgePosition(EdgePosition const& other) : bits_(other.bits_) {}
	void swap(EdgePosition& other) {
		using std::swap;
		swap(bits_, other.bits_);
	}
	EdgePosition(EdgePosition&& other) noexcept {
		swap(other);
	}
	EdgePosition& operator=(EdgePosition const& other) {
		if (this != &other) {
			bits_ = other.bits_;
		}
		return *this;
	}
	EdgePosition& operator=(EdgePosition&& other) noexcept {
		swap(other);
		return *this;
	}
	void swap_bits(uint8_t, uint8_t) override {
		bits_ = (bits_ << COLOURS_COUNT) | (bits_ >> COLOURS_COUNT);
	}
	Positions operator[](uint8_t i) const override {
		std::bitset<COLOURS_COUNT * 2> bit_mask(0b111111);
		return static_cast<Positions>(((bits_ >> (COLOURS_COUNT * (1 - i))) & bit_mask).to_ulong());
	}
private:
	std::bitset<COLOURS_COUNT * 2> bits_;
};



#endif