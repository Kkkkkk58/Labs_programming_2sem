#include "GeneticCandidate.hpp"
#include "GeneticMutations.hpp"

// Конструктор класса Candidate от состояния кубика
Candidate::Candidate(RubiksCube const& cube) : cube_(cube), moves_(), updated_(true), curr_fitness_(0) {}

// Конструктор копирования класса Candidate
Candidate::Candidate(Candidate const& other)
	: cube_(other.cube_), moves_(other.moves_), updated_(other.updated_), curr_fitness_(other.curr_fitness_) {}

// Метод swap класса Candidate
void Candidate::swap(Candidate& other) {
	using std::swap;
	swap(cube_, other.cube_);
	swap(moves_, other.moves_);
	swap(updated_, other.updated_);
	swap(curr_fitness_, other.curr_fitness_);
}

// Конструктор перемещения класса Candidate
Candidate::Candidate(Candidate&& other) noexcept {
	swap(other);
}

// Оператор присваивания класса Candidate
Candidate& Candidate::operator=(Candidate const& other) {
	if (this != &other) {
		cube_ = other.cube_;
		moves_ = other.moves_;
		updated_ = other.updated_;
		curr_fitness_ = other.curr_fitness_;
	}
	return *this;
}

// Перемещающий оператор присваивания класса Candidate
Candidate& Candidate::operator=(Candidate&& other) noexcept {
	swap(other);
	return *this;
}

// Метод исполнения последовательности поворотов по коду
void Candidate::perform_sequence(uint8_t sequence_code) {
	cube_.setup(mutations_sequences[sequence_code]);
	moves_.push_back(sequence_code);
	updated_ = true;
}

// Метод раскодирования списка мутаций
MoveSequence Candidate::get_moves() const {
	MoveSequence moves;
	for (auto const& move : moves_) {
		moves += mutations_sequences[move];
	}
	return moves;
}

// Метод ленивого пересчета параметра fitness
size_t Candidate::fitness() {
	if (updated_) {
		updated_ = false;
		curr_fitness_ = STICKERS_COUNT - wrong_stickers();
	}
	return curr_fitness_;
}

// Метод доступа к полю cube_ класса Candidate
RubiksCube const& Candidate::get_cube() const {
	return cube_;
}

// Метод пересчета количества стикеров, находящихся не на своих местах
size_t Candidate::wrong_stickers() const {
	using namespace RubiksCubeHelper;
	// Проверка всех кубиков
	size_t count = check(UP_BACK_LEFT, UP, BACK, LEFT) + check(UP_BACK, UP, BACK) + check(UP_BACK_RIGHT, UP, BACK, RIGHT)
		+ check(UP_RIGHT, UP, RIGHT) + check(UP_FRONT_RIGHT, UP, FRONT, RIGHT) + check(UP_FRONT, UP, FRONT)
		+ check(UP_FRONT_LEFT, UP, FRONT, LEFT) + check(UP_LEFT, UP, LEFT) + check(FRONT_LEFT, FRONT, LEFT)
		+ check(BACK_LEFT, BACK, LEFT) + check(BACK_RIGHT, BACK, RIGHT) + check(FRONT_RIGHT, FRONT, RIGHT)
		+ check(DOWN_BACK_LEFT, DOWN, BACK, LEFT) + check(DOWN_BACK, DOWN, BACK) + check(DOWN_BACK_RIGHT, DOWN, BACK, RIGHT)
		+ check(DOWN_RIGHT, DOWN, RIGHT) + check(DOWN_FRONT_RIGHT, DOWN, FRONT, RIGHT) + check(DOWN_FRONT, DOWN, FRONT)
		+ check(DOWN_FRONT_LEFT, DOWN, FRONT, LEFT) + check(DOWN_LEFT, DOWN, LEFT);
	return count;
}

// Проверка углового кубика
size_t Candidate::check(RubiksCubeHelper::CornersIndexing el, RubiksCubeHelper::CentersIndexing a,
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

// Проверка реберного кубика
size_t Candidate::check(RubiksCubeHelper::EdgesIndexing el, RubiksCubeHelper::CentersIndexing a,
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
