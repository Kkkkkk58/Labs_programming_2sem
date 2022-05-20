#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP
#include "Cube.hpp"
#include "Solution.hpp"
#include "GeneticCandidate.hpp"

class GeneticAlgorithm : public ISolution {
public:
	explicit GeneticAlgorithm(RubiksCube const& cube);
	void solve() override;
	MoveSequence sequence() const override;
private:
	RubiksCube initial_state_;
	std::vector<Candidate> population_;
	bool found_solution;
	static const size_t DEATH_AGE, POPULATION_NUMBER;
	void reset();
	size_t get_combination() const;
};

#endif