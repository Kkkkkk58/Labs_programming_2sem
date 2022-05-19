#ifndef SOLUTION_HPP
#define SOLUTION_HPP
#include "Cube.hpp"
#include <random>
#include <string>


class ISolution {
public:
	virtual ~ISolution() {}
	virtual void solve() = 0;
	virtual MoveSequence sequence() const = 0;
};



class Candidate {
public:
	explicit Candidate(RubiksCube const& cube) : cube_(cube), moves_(), updated_(true), curr_fitness_(0) {}
	Candidate(Candidate const& other)
		: cube_(other.cube_), moves_(other.moves_), updated_(other.updated_), curr_fitness_(other.curr_fitness_) {}
	void swap(Candidate& other) {
		using std::swap;
		swap(cube_, other.cube_);
		swap(moves_, other.moves_);
		swap(updated_, other.updated_);
		swap(curr_fitness_, other.curr_fitness_);
	}
	Candidate(Candidate&& other) noexcept {
		swap(other);
	}
	Candidate& operator=(Candidate const& other) {
		if (this != &other) {
			cube_ = other.cube_;
			moves_ = other.moves_;
			updated_ = other.updated_;
			curr_fitness_ = other.curr_fitness_;
		}
		return *this;
	}
	Candidate& operator=(Candidate&& other) noexcept {
		swap(other);
		return *this;
	}
	~Candidate() = default;
	void perform_sequence(MoveSequence const& sequence) {
		cube_.setup(sequence);
		moves_ += sequence;
		updated_ = true;
	}
	MoveSequence const& get_moves() const {
		return moves_;
	}
	size_t fitness() {
		if (updated_) {
			updated_ = false;
			curr_fitness_ = STICKERS_COUNT - wrong_stickers();
		}
		return curr_fitness_;
	}
	void setup(MoveSequence const& move) {
		cube_.setup(move);
	}
	operator RubiksCube() {
		return cube_;
	}
private:
	RubiksCube cube_;
	MoveSequence moves_;
	bool updated_;
	size_t curr_fitness_;
	/* 	enum CornersIndexing : uint8_t {
		UP_BACK_LEFT, UP_BACK_RIGHT, UP_FRONT_RIGHT, UP_FRONT_LEFT, 
		DOWN_FRONT_LEFT, DOWN_BACK_LEFT, DOWN_BACK_RIGHT, DOWN_FRONT_RIGHT
	};

	enum EdgesIndexing : uint8_t {
		UP_BACK, UP_RIGHT, UP_FRONT, UP_LEFT, 
		FRONT_LEFT, BACK_LEFT, BACK_RIGHT, FRONT_RIGHT, 
		DOWN_LEFT, DOWN_BACK, DOWN_RIGHT, DOWN_FRONT
	};*/
	size_t wrong_stickers() const {
		using namespace RubiksCubeHelper;
		using P = Position::Positions;
		size_t count = check(UP_BACK_LEFT, UP, BACK, LEFT) + check(UP_BACK, UP, BACK) + check(UP_BACK_RIGHT, UP, BACK, RIGHT)
			+ check(UP_RIGHT, UP, RIGHT) + check(UP_FRONT_RIGHT, UP, FRONT, RIGHT) + check(UP_FRONT, UP, FRONT) 
			+ check(UP_FRONT_LEFT, UP, FRONT, LEFT) + check(UP_LEFT, UP, LEFT) + check(FRONT_LEFT, FRONT, LEFT) 
			+ check(BACK_LEFT, BACK, LEFT) + check(BACK_RIGHT, BACK, RIGHT) + check(FRONT_RIGHT, FRONT, RIGHT)
			+ check(DOWN_BACK_LEFT, DOWN, BACK, LEFT) + check(DOWN_BACK, DOWN, BACK) + check(DOWN_BACK_RIGHT, DOWN, BACK, RIGHT)
			+ check(DOWN_RIGHT, DOWN, RIGHT) + check(DOWN_FRONT_RIGHT, DOWN, FRONT, RIGHT) + check(DOWN_FRONT, DOWN, FRONT)
			+ check(DOWN_FRONT_LEFT, DOWN, FRONT, LEFT) + check(DOWN_LEFT, DOWN, LEFT);
		return count;
	}
	size_t check(RubiksCubeHelper::CornersIndexing el, RubiksCubeHelper::CentersIndexing a, 
		RubiksCubeHelper::CentersIndexing b, RubiksCubeHelper::CentersIndexing c) const {
		size_t count = 0;
		if (cube_.corner(el)[0] != cube_.face(a)) {
			++count;
		}
		if (cube_.corner(el)[1] != cube_.face(b)) {
			++count;
		}
		if (cube_.corner(el)[2] != cube_.face(c)) {
			++count;
		}
		return count;
	}
	size_t check(RubiksCubeHelper::EdgesIndexing el, RubiksCubeHelper::CentersIndexing a, 
		RubiksCubeHelper::CentersIndexing b) const {
		size_t count = 0;
		if (cube_.edge(el)[0] != cube_.face(a)) {
			++count;
		}
		if (cube_.edge(el)[1] != cube_.face(b)) {
			++count;
		}
		return count;
	}
};


class GeneticAlgorithm : public ISolution {
public:
	explicit GeneticAlgorithm(RubiksCube const& cube)
		: ISolution(), initial_state_(cube),
		population_(std::vector<Candidate>(1000, Candidate(initial_state_))), found_solution(false) {}
	//TODO: Multithreading

	void solve() override {
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x2);
		std::cout << "\n\n\t\t\t\t\t\tCalculating...\n\n\t\t\t\t";
		for (size_t i = 0; i < 54; ++i) {
			std::cout << (char)177;
		}
		std::cout << "\r\t\t\t\t";
		while (true) {
			size_t curr_rate = 0;
			for (size_t i = 0; i < DEATH_AGE; ++i) {
				for (auto& cube : population_) {
					cube.perform_sequence(mutations_sequences[get_combination()]);
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
					
					//std::cout << (char)219;
					//std::cout << i << " " << population_[0].fitness() << "\n";
					//if (population_[0].fitness() == 48) {
					//	std::cout << population_[0];
					//}
				}
				std::copy(population_.begin(), population_.begin() + 151, population_.begin() + 151);
				std::copy(population_.begin(), population_.begin() + 301, population_.begin() + 301);
				std::copy(population_.begin(), population_.begin() + 500, population_.begin() + 500);

			}
			if (!found_solution) {
				reset();
			}
			else {
				break;
			}
		}
	}

	MoveSequence sequence() const override {
		if (found_solution) {
			return population_[0].get_moves();
		}
		return MoveSequence();
	}

	void reset() {
		population_ = std::vector<Candidate>(1000, Candidate(initial_state_));
	}

private:
	RubiksCube initial_state_;
	std::vector<Candidate> population_;
	static const size_t DEATH_AGE;
	static const std::vector<MoveSequence> mutations_sequences;
	size_t get_combination() const {
		std::random_device device;
		std::mt19937 generator(device());
		return generator() % mutations_sequences.size();
	}
	bool found_solution;
};

const size_t GeneticAlgorithm::DEATH_AGE = 100;
const std::vector<MoveSequence> GeneticAlgorithm::mutations_sequences = {
	// permutes two edges : U face, bottom edgeand right edge
	MoveSequence("F' L' B' R' U' R U' B L F R U R' U"),
	// permutes two edges : U face, bottom edgeand left edge
	MoveSequence("F R B L U L' U B' R' F' L' U' L U'"),
	// permutes two corners : U face, bottom leftand bottom right
	MoveSequence("U2 B U2 B' R2 F R' F' U2 F' U2 F R'"),
	// permutes three corners : U face, bottom leftand top left
	MoveSequence("U2 R U2 R' F2 L F' L' U2 L' U2 L F'"),
	// permutes three centers : F face, top, right, bottom
	MoveSequence("U' B2 D2 L' F2 D2 B2 R' U'"),
	// permutes three centers : F face, top, right, left
	MoveSequence("U B2 D2 R F2 D2 B2 L U"),
	// U face : bottom edge < ->right edge, bottom right corner < ->top right corner
	MoveSequence("D' R' D R2 U' R B2 L U' L' B2 U R2"),
	// U face : bottom edge < ->right edge, bottom right corner < ->left right corner
	MoveSequence("D L D' L2 U L' B2 R' U R B2 U' L2"),
	// U face : top edge < ->bottom edge, bottom left corner < ->top right corner
	MoveSequence("R' U L' U2 R U' L R' U L' U2 R U' L U'"),
	// U face : top edge < ->bottom edge, bottom right corner < ->top left corner
	MoveSequence("L U' R U2 L' U R' L U' R U2 L' U R' U"),
	// permutes three corners : U face, bottom right, bottom leftand top left
	MoveSequence("F' U B U' F U B' U'"),
	// permutes three corners : U face, bottom left, bottom rightand top right
	MoveSequence("F U' B' U F' U' B U"),
	// permutes three edges : F face bottom, F face top, B face top
	MoveSequence("L' U2 L R' F2 R"),
	// permutes three edges : F face top, B face top, B face bottom
	MoveSequence("R' U2 R L' B2 L"),
	// H permutation : U Face, swaps the edges horizontallyand vertically
	MoveSequence("M2 U M2 U2 M2 U M2"),
	MoveSequence("X"), MoveSequence("X'"), MoveSequence("X2"),
	MoveSequence("Y"), MoveSequence("Y2"), MoveSequence("Y'"), 
	MoveSequence("Z"), MoveSequence("Z'"), MoveSequence("Z2"),
	MoveSequence("U R L' F2 R L' U2 D' L2 F2 U' L2 B2 L2 U' F2"),
	MoveSequence("U2 R' U2 F2 U2 R U D' F2 R2 B2 L2 U' D' B2 R2"),
	MoveSequence("U R' L U2 F2 R' L' D' F2 U' R2 U R2 U2 B2 U' L2")
};



// TODO
//class BeginnersMethod : public ISolution {
// };
//class Kociemba : public ISolution {
//
//};
//class Friedrich : public ISolution {
//};
//class Korf : public ISolution {
// 
//};
// class Thistlewaite : public ISolution {
// 
// };
// class Pochmann : public ISolution {
// 
// }
//class GeneticAlgorithm : public ISolution {
//
//};

#endif
