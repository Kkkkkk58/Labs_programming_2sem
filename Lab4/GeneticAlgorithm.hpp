#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP
#include "Cube.hpp"
#include "Solution.hpp"
#include "GeneticCandidate.hpp"

// Класс-"решатель" кубика Рубика генетическим алгоритмом
class GeneticAlgorithm : public ISolution {
public:
	explicit GeneticAlgorithm(RubiksCube const& cube);
	void solve() override;
	MoveSequence sequence() const override;
private:
	RubiksCube initial_state_;				// Сохранение исходного состояния кубика (если понадобится новая популяция)
	std::vector<Candidate> population_;		// Текущая популяция
	bool found_solution;					// Флаг, сигнализирующий о том, что решение найдено
	static const size_t DEATH_AGE, POPULATION_NUMBER;	// Константы - размер популяции и номер, после которого создается новая
	void reset();
	size_t get_combination() const;
};

#endif