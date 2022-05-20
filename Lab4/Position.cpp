#include "Position.hpp"


CornerPosition::CornerPosition() : bits_() {}

CornerPosition::CornerPosition(Positions const& a, Positions const& b, Positions const& c)
	: bits_((static_cast<uint16_t>(a) << (COLOURS_COUNT * 2)) | (static_cast<uint16_t>(b) << COLOURS_COUNT) | static_cast<uint8_t>(c)) {}
CornerPosition::CornerPosition(CornerPosition const& other) : bits_(other.bits_) {}

void CornerPosition::swap(CornerPosition& other) {
	using std::swap;
	swap(bits_, other.bits_);
}
CornerPosition::CornerPosition(CornerPosition&& other) noexcept {
	swap(other);
}
CornerPosition& CornerPosition::operator=(CornerPosition const& other) {
	if (this != &other) {
		bits_ = other.bits_;
	}
	return *this;
}
CornerPosition& CornerPosition::operator=(CornerPosition&& other) noexcept {
	swap(other);
	return *this;
}

void CornerPosition::swap_bits(uint8_t a, uint8_t b) {
	if (a < b) {
		std::swap(a, b);
	}
	std::bitset<COLOURS_COUNT * 3> first_bit_mask((0b111111 << ((2 - a) * COLOURS_COUNT)));
	std::bitset<COLOURS_COUNT * 3> second_bit_mask((0b111111 << ((2 - b) * COLOURS_COUNT)));
	if ((a | b) == 0b10) {
		uint8_t pentabits_avail = 1;
		std::bitset<COLOURS_COUNT * 3> third_bit_mask = 0b111111 << (pentabits_avail * COLOURS_COUNT);
		bits_ = ((bits_ & first_bit_mask) << COLOURS_COUNT * 2) | ((bits_ & second_bit_mask) >> COLOURS_COUNT * 2) | (bits_ & third_bit_mask);
	}
	else {
		uint8_t pentabits_avail = ((a | b) == 0b1) ? 0 : 2;
		std::bitset<COLOURS_COUNT * 3> third_bit_mask = 0b111111 << (pentabits_avail * COLOURS_COUNT);
		bits_ = ((bits_ & first_bit_mask) << COLOURS_COUNT) | ((bits_ & second_bit_mask) >> COLOURS_COUNT) | (bits_ & third_bit_mask);
	}
}

Position::Positions CornerPosition::operator[](uint8_t i) const {
	std::bitset<COLOURS_COUNT * 3> bit_mask(0b111111);
	return static_cast<Positions>(((bits_ >> (COLOURS_COUNT * (2 - i))) & bit_mask).to_ulong());
}



EdgePosition::EdgePosition() : bits_() {}

EdgePosition::EdgePosition(Positions const& a, Positions const& b)
	: bits_((static_cast<uint16_t>(a) << COLOURS_COUNT) | static_cast<uint8_t>(b)) {}

EdgePosition::EdgePosition(EdgePosition const& other) : bits_(other.bits_) {}

void EdgePosition::swap(EdgePosition& other) {
	using std::swap;
	swap(bits_, other.bits_);
}


EdgePosition::EdgePosition(EdgePosition&& other) noexcept {
	swap(other);
}

EdgePosition& EdgePosition::operator=(EdgePosition const& other) {
	if (this != &other) {
		bits_ = other.bits_;
	}
	return *this;
}

EdgePosition& EdgePosition::operator=(EdgePosition&& other) noexcept {
	swap(other);
	return *this;
}

void EdgePosition::swap_bits(uint8_t, uint8_t) {
	bits_ = (bits_ << COLOURS_COUNT) | (bits_ >> COLOURS_COUNT);
}

Position::Positions EdgePosition::operator[](uint8_t i) const {
	std::bitset<COLOURS_COUNT * 2> bit_mask(0b111111);
	return static_cast<Positions>(((bits_ >> (COLOURS_COUNT * (1 - i))) & bit_mask).to_ulong());
}