#ifndef CUBE_VALIDATOR_HPP
#define CUBE_VALIDATOR_HPP
#include <unordered_map>
#include <vector>
#include "Cube.hpp"

// Класс-валидатор кубика
class CubeValidator {
public:
	CubeValidator(RubiksCube const&);
	void report();
private:
	RubiksCube cube_;		// Экземпляр куба для проверки
	static std::unordered_map<Position::Positions, Position::Positions> opposite_colours;  // Хеш-таблица противоположных цветов
	bool correct_centers() const;
	bool correct_count() const;
	bool corners_invariant() const;
	bool edges_invariant() const;
	bool permutations_invariant() const;
	size_t edges_perm(std::vector<Colour> const& centers) const;
	size_t corners_perm(std::vector<Colour> const& centers) const;
};
#endif
