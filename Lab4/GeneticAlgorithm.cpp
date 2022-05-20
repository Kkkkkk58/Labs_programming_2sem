#include "GeneticAlgorithm.hpp"
#include "GeneticMutations.hpp"
#include <Windows.h>
#include <algorithm>
#include <random>

GeneticAlgorithm::GeneticAlgorithm(RubiksCube const& cube)
	: ISolution(), initial_state_(cube),
	population_({ Candidate(cube) }), found_solution(false) {
	population_[0].fitness();
}


void GeneticAlgorithm::solve() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x2);
	std::cout << "\n\n\t\t\t\t\t\t   Calculating...\n\n\t\t\t\t";
	for (size_t i = 0; i < 54; ++i) {
		std::cout << (char)177;
	}
	if (population_[0].fitness() == STICKERS_COUNT) {
		std::cout << "\r\t\t\t\t";
		for (size_t i = 0; i < 54; ++i) {
			std::cout << (char)219;
		}
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
		found_solution = true;
	}
	reset();
	std::clock_t start;
	double duration;
	start = std::clock();
	while (!found_solution) {
		std::cout << "\r\t\t\t\t";
		size_t curr_rate = 0;
		for (size_t i = 0; i < DEATH_AGE; ++i) {
			for (auto& cube : population_) {
				cube.perform_sequence(get_combination());
			}
			std::sort(population_.begin(), population_.end(),
				[](Candidate& a, Candidate& b) { return a.fitness() > b.fitness(); });
			if (population_[0].fitness() == STICKERS_COUNT) {
				for (size_t i = 0; i < population_[0].fitness() - curr_rate; ++i) {
					std::cout << (char)219;
				}
				std::cout << "\n\n\n";
				SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);

				found_solution = true;
				break;
			}
			else {
				if (population_[0].fitness() > curr_rate) {
					for (size_t i = 0; i < population_[0].fitness() - curr_rate; ++i) {
						std::cout << (char)219;
					}
					curr_rate = population_[0].fitness();
				};
			}
			std::copy(population_.begin(), population_.begin() + 16, population_.begin() + 16);
			std::copy(population_.begin(), population_.begin() + 31, population_.begin() + 31);
			std::copy(population_.begin(), population_.begin() + 50, population_.begin() + 50);

		}
		if (!found_solution) {
			reset();
		}
		else {
			duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
			std::cout << "\t\t\t\t\t\tTime elapsed: " << duration << "\n\n\n";
			break;
		}
	}
}

MoveSequence GeneticAlgorithm::sequence() const {
	if (found_solution) {
		return population_[0].get_moves();
	}
	return MoveSequence();
}

void GeneticAlgorithm::reset() {
	population_ = std::vector<Candidate>(100, Candidate(initial_state_));
}


size_t GeneticAlgorithm::get_combination() const {
	std::random_device device;
	std::mt19937 generator(device());
	return generator() % mutations_sequences.size();
}


const size_t GeneticAlgorithm::DEATH_AGE = 100, GeneticAlgorithm::POPULATION_NUMBER = 100;