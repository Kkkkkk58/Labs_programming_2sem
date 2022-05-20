#ifndef CUBE_VALIDATOR_HPP
#define CUBE_VALIDATOR_HPP
#include <unordered_map>
#include <vector>
#include "Colour.hpp"
#include "Position.hpp"
#include "Cube.hpp"


class CubeValidator {
public:
	CubeValidator(RubiksCube const&);
	void report();
private:
	static std::unordered_map<Position::Positions, Position::Positions> opposite_colours;
	bool correct_centers() const;
	bool correct_count() const;
	bool corners_invariant() const;
	bool edges_invariant() const;
	bool permutations_invariant() const;
	size_t edges_perm(std::vector<Colour> const& centers) const;
	size_t corners_perm(std::vector<Colour> const& centers) const;
	RubiksCube cube_;
};
#endif
