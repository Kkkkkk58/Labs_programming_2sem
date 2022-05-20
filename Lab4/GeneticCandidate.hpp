#ifndef GENETIC_CANDIDATE_HPP
#define GENETIC_CANDIDATE_HPP
#include "Cube.hpp"

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
	void setup(MoveSequence const&);
	RubiksCube const& get_cube() const;
private:
	RubiksCube cube_;
	std::vector<uint8_t> moves_;
	bool updated_;
	size_t curr_fitness_;
	size_t wrong_stickers() const;
	size_t check(RubiksCubeHelper::CornersIndexing el, RubiksCubeHelper::CentersIndexing a,
		RubiksCubeHelper::CentersIndexing b, RubiksCubeHelper::CentersIndexing c) const;
	size_t check(RubiksCubeHelper::EdgesIndexing el, RubiksCubeHelper::CentersIndexing a,
		RubiksCubeHelper::CentersIndexing b) const;
};



#endif
