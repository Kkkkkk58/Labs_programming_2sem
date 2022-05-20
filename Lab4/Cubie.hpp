#ifndef CUBIE_HPP
#define CUBIE_HPP
#include <vector>
#include "Colour.hpp"
#include "Position.hpp"
#include "Moves.hpp"

class Cubie {
public:
	virtual ~Cubie() {}
	virtual Position::Positions operator[](uint8_t) const = 0;
	virtual void rotate(Move const&) = 0;
};


class CenterCubie : public Cubie {
public:
	enum Faces : uint8_t {
		UP, LEFT, FRONT, RIGHT, BACK, DOWN
	};
	explicit CenterCubie(std::vector<Colour> const& = { C::WHITE, C::ORANGE, C::GREEN, C::RED, C::BLUE, C::YELLOW });
	CenterCubie(CenterCubie const&);
	void swap(CenterCubie&);
	CenterCubie(CenterCubie&&) noexcept;
	CenterCubie& operator=(CenterCubie const&);
	CenterCubie& operator=(CenterCubie&&) noexcept;
	Position::Positions operator[](uint8_t) const override;
	Colour& operator[](uint8_t);
	std::vector<Colour> const& get_colours() const;
	std::vector<Colour>& get_colours();
	void rotate(Move const&) override;
private:
	std::vector<Colour> colours_;
	void rotate_x(uint8_t, bool);
	void rotate_y(uint8_t, bool);
	void rotate_z(uint8_t, bool);
	using C = Colour::Colours;
};


class EdgeCubie : public Cubie {
public:
	explicit EdgeCubie(EdgePosition const& = EdgePosition(Position::Positions::WHITE, Position::Positions::BLUE));
	EdgeCubie(EdgeCubie const&);
	void swap(EdgeCubie&);
	EdgeCubie(EdgeCubie&&) noexcept;
	EdgeCubie& operator=(EdgeCubie const&);
	EdgeCubie& operator=(EdgeCubie&&) noexcept;
	EdgePosition const& get_orientation() const;
	Position::Positions operator[](uint8_t i) const override;
	std::vector<Colour> get_colours() const;
	void rotate(Move const&) override;
private:
	EdgePosition orientation_;
};
 

class CornerCubie : public Cubie {
public:
	explicit CornerCubie(CornerPosition const& = CornerPosition(Position::Positions::WHITE, \
		Position::Positions::RED, Position::Positions::BLUE));
	CornerCubie(CornerCubie const&);
	void swap(CornerCubie&);
	CornerCubie(CornerCubie&&) noexcept;
	CornerCubie& operator=(CornerCubie const&);
	CornerCubie& operator=(CornerCubie&&) noexcept;
	CornerPosition const& get_orientation() const;
	Position::Positions operator[](uint8_t i) const override;
	void rotate(Move const&);
	std::vector<Colour> get_colours() const;
private:
	CornerPosition orientation_;
};

#endif
