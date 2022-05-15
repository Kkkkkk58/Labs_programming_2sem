#ifndef COLOUR_HPP
#define COLOUR_HPP
#include <cinttypes>
#include "CubeExceptions.hpp"
#include <iostream>
#include <Windows.h>


class Colour {
public:
	enum class Colours : uint8_t {
		WHITE, BLUE, RED, GREEN, YELLOW, ORANGE
	};
	Colour(Colours colour = Colours::WHITE) : colour_(colour) {}
	Colour(char colour) {
		switch (colour) {
		case 'W': case 'w':
			colour_ = Colours::WHITE;
			break;
		case 'B': case 'b':
			colour_ = Colours::BLUE;
			break;
		case 'R': case 'r':
			colour_ = Colours::RED;
			break;
		case 'G': case 'g':
			colour_ = Colours::GREEN;
			break;
		case 'Y': case 'y':
			colour_ = Colours::YELLOW;
			break;
		case 'O': case 'o':
			colour_ = Colours::ORANGE;
			break;
		default: throw UnknownColour(std::string(1, colour));
		}
	}
	~Colour() = default;
	Colour(Colour const& other) : colour_(other.colour_) {}
	void swap(Colour& other) {
		using std::swap;
		swap(colour_, other.colour_);
	}
	Colour(Colour&& other) noexcept : colour_(std::move(other.colour_)) {}
	Colour& operator=(Colour const& other) {
		if (this != &other) {
			colour_ = other.colour_;
		}
		return *this;
	}
	Colour& operator=(Colour&& other) noexcept {
		swap(other);
		return *this;
	}
	bool operator==(Colour const& other) const {
		return colour_ == other.colour_;
	}
private:
	Colours colour_;
};


std::ostream& operator<<(std::ostream& os, Colour::Colours const& colour) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (colour) {
	case Colour::Colours::WHITE:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
		os << 'W';
		break;
	case Colour::Colours::BLUE:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x1);
		os << 'B';
		break;
	case Colour::Colours::GREEN:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x2);
		os << 'G';
		break;
	case Colour::Colours::ORANGE:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0xE);
		os << 'O';
		break;
	case Colour::Colours::RED:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x4);
		os << 'R';
		break;
	case Colour::Colours::YELLOW:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x6);
		os << 'Y';
		break;
	}
	SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
	return os;
}

#endif
