#include "Position.hpp"

// Конструктор по умолчанию класса CornerPosition
CornerPosition::CornerPosition() : bits_() {}

// Конструктор класса CornerPosition от трех элементов enum class Positions
CornerPosition::CornerPosition(Positions const& a, Positions const& b, Positions const& c)
	// Побитово складываем три "цвета"
	: bits_((static_cast<uint16_t>(a) << (COLOURS_COUNT * 2)) | (static_cast<uint16_t>(b) << COLOURS_COUNT) | static_cast<uint8_t>(c)) {}

// Конструктор копирования класса CornerPosition
CornerPosition::CornerPosition(CornerPosition const& other) : bits_(other.bits_) {}

// Метод swap класса CornerPosition
void CornerPosition::swap(CornerPosition& other) {
	using std::swap;
	swap(bits_, other.bits_);
}

// Конструктор перемещения класса CornerPosition
CornerPosition::CornerPosition(CornerPosition&& other) noexcept {
	swap(other);
}

// Оператор присваивания класса CornerPosition
CornerPosition& CornerPosition::operator=(CornerPosition const& other) {
	if (this != &other) {
		bits_ = other.bits_;
	}
	return *this;
}

// Перемещающий оператор присваивания класса CornerPosition
CornerPosition& CornerPosition::operator=(CornerPosition&& other) noexcept {
	swap(other);
	return *this;
}

// Метод swap_bits для изменения относительного порядка битов в битсете для класса CornerPosition
void CornerPosition::swap_bits(uint8_t a, uint8_t b) {
	if (a < b) {
		std::swap(a, b);
	}
	// Битовая маска для первого цвета
	std::bitset<COLOURS_COUNT * 3> first_bit_mask((0b111111 << ((2 - a) * COLOURS_COUNT)));
	// Битовая маска для второго цвета
	std::bitset<COLOURS_COUNT * 3> second_bit_mask((0b111111 << ((2 - b) * COLOURS_COUNT)));

	// Обмен значениями первого и последнего цвета
	if ((a | b) == 0b10) {
		uint8_t sixbits_avail = 1;
		// Маска для цвета, остающегося на месте
		std::bitset<COLOURS_COUNT * 3> third_bit_mask = 0b111111 << (sixbits_avail * COLOURS_COUNT);
		bits_ = ((bits_ & first_bit_mask) << COLOURS_COUNT * 2) | ((bits_ & second_bit_mask) >> COLOURS_COUNT * 2) | (bits_ & third_bit_mask);
	}
	else {
		uint8_t sixbits_avail = ((a | b) == 0b1) ? 0 : 2;
		// Маска для цвета, остающегося на месте
		std::bitset<COLOURS_COUNT * 3> third_bit_mask = 0b111111 << (sixbits_avail * COLOURS_COUNT);
		bits_ = ((bits_ & first_bit_mask) << COLOURS_COUNT) | ((bits_ & second_bit_mask) >> COLOURS_COUNT) | (bits_ & third_bit_mask);
	}
}

// Оператор доступа к цвету по его номеру 
Position::Positions CornerPosition::operator[](uint8_t i) const {
	std::bitset<COLOURS_COUNT * 3> bit_mask(0b111111);
	return static_cast<Positions>(((bits_ >> (COLOURS_COUNT * (2 - i))) & bit_mask).to_ulong());
}


// Конструктор по умолчанию класса EdgePosition
EdgePosition::EdgePosition() : bits_() {}

// Конструктор класса EdgePosition от двух элементов enum class Position
EdgePosition::EdgePosition(Positions const& a, Positions const& b)
	// Выполняем побитовое сложение
	: bits_((static_cast<uint16_t>(a) << COLOURS_COUNT) | static_cast<uint8_t>(b)) {}

// Конструктор копирования класса EdgePosition
EdgePosition::EdgePosition(EdgePosition const& other) : bits_(other.bits_) {}

// Метод swap класса EdgePosition
void EdgePosition::swap(EdgePosition& other) {
	using std::swap;
	swap(bits_, other.bits_);
}

// Конструктор перемещения класса EdgePosition
EdgePosition::EdgePosition(EdgePosition&& other) noexcept {
	swap(other);
}

// Оператор присваивания класса EdgePosition
EdgePosition& EdgePosition::operator=(EdgePosition const& other) {
	if (this != &other) {
		bits_ = other.bits_;
	}
	return *this;
}

// Перемещающий оператор присваивания класса EdgePosition
EdgePosition& EdgePosition::operator=(EdgePosition&& other) noexcept {
	swap(other);
	return *this;
}

// Метод swap_bits для изменения относительного порядка битов в битсете для класса EdgePosition
void EdgePosition::swap_bits(uint8_t, uint8_t) {
	bits_ = (bits_ << COLOURS_COUNT) | (bits_ >> COLOURS_COUNT);
}

// Оператор доступа к цвету по его номеру 
Position::Positions EdgePosition::operator[](uint8_t i) const {
	std::bitset<COLOURS_COUNT * 2> bit_mask(0b111111);
	return static_cast<Positions>(((bits_ >> (COLOURS_COUNT * (1 - i))) & bit_mask).to_ulong());
}