#ifndef COLOUR_HPP
#define COLOUR_HPP
#include <cinttypes>
#include "CubeExceptions.hpp"
#include <iostream>
#include <Windows.h>
#include "Position.hpp"

class Colour {
public:
	enum class Colours : uint8_t {
		WHITE, BLUE, RED, GREEN, YELLOW, ORANGE
	};
	Colour(Colours colour = Colours::WHITE) : colour_(colour) {}
	Colour(Position::Positions const& pos) : colour_(Colours::WHITE) {
		switch (pos) {
		case Position::Positions::WHITE:
			colour_ = Colours::WHITE;
			break;
		case Position::Positions::BLUE:
			colour_ = Colours::BLUE;
			break;
		case Position::Positions::RED:
			colour_ = Colours::RED;
			break;
		case Position::Positions::GREEN:
			colour_ = Colours::GREEN;
			break;
		case Position::Positions::YELLOW:
			colour_ = Colours::YELLOW;
			break;
		case Position::Positions::ORANGE:
			colour_ = Colours::ORANGE;
			break;
		default: break;
		}
	}
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
	operator Position::Positions() const {
		switch (colour_) {
		case Colours::WHITE: return Position::Positions::WHITE;
		case Colours::BLUE: return Position::Positions::BLUE;
		case Colours::GREEN: return Position::Positions::GREEN;
		case Colours::ORANGE: return Position::Positions::ORANGE;
		case Colours::RED: return Position::Positions::RED;
		default: return Position::Positions::YELLOW;
		}
	}
	operator char() const {
		switch(colour_) {
		case Colours::WHITE: return 'W';
		case Colours::BLUE: return 'B';
		case Colours::GREEN: return 'G';
		case Colours::ORANGE: return 'O';
		case Colours::RED: return 'R';
		default: return 'Y';
		}
	}
private:
	Colours colour_;
};


std::ostream& operator<<(std::ostream& os, Position::Positions const& colour) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (colour) {
	case Position::Positions::WHITE:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
		os << 'W';
		break;
	case Position::Positions::BLUE:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x1);
		os << 'B';
		break;
	case Position::Positions::GREEN:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x2);
		os << 'G';
		break;
	case Position::Positions::ORANGE:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0xE);
		os << 'O';
		break;
	case Position::Positions::RED:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x4);
		os << 'R';
		break;
	case Position::Positions::YELLOW:
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x6);
		os << 'Y';
		break;
	default:
		break;
	}
	SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
	return os;
}

std::istream& operator>>(std::istream& is, Colour& colour) {
	char input;
	is >> input;
	colour = input;
	return is;
}
#endif
