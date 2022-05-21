#ifndef GENETIC_CANDIDATE_HPP
#define GENETIC_CANDIDATE_HPP
#include "Cube.hpp"

// Класс члена популяции для генетического алгоритма
class Candidate {
public:
	explicit Candidate(RubiksCube const&);
	Candidate(Candidate const&);
	void swap(Candidate&);
	Candidate(Candidate&&) noexcept;
	Candidate& operator=(Candidate const&);
	Candidate& operator=(Candidate&&) noexcept;
	~Candidate() = default;
	void perform_sequence(uint8_t);
	MoveSequence get_moves() const;
	size_t fitness();
	RubiksCube const& get_cube() const;
private:
	RubiksCube cube_;				// Состояние куба, представляемое данным кандидатом
	std::vector<uint8_t> moves_;	// Закодированная последовательность мутаций, произошедших с кандидатом
	bool updated_;					// Статус обновления для ленивого пересчета параметра fitness
	size_t curr_fitness_;			// Поле fitness, характеризующее, насколько кандидат годен к отбору
	size_t wrong_stickers() const;
	size_t check(RubiksCubeHelper::CornersIndexing el, RubiksCubeHelper::CentersIndexing a,
		RubiksCubeHelper::CentersIndexing b, RubiksCubeHelper::CentersIndexing c) const;
	size_t check(RubiksCubeHelper::EdgesIndexing el, RubiksCubeHelper::CentersIndexing a,
		RubiksCubeHelper::CentersIndexing b) const;
};



#endif
