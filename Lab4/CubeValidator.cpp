#include "CubeValidator.hpp"
#include "RubiksCubeHelper.hpp"
#include "CubeExceptions.hpp"
#include <algorithm>

CubeValidator::CubeValidator(RubiksCube const& cube) : cube_(cube) {}

void CubeValidator::report() {
	if (!correct_centers() || !correct_count()) {
		throw InvalidState("Incorrect stickering");
	}
	if (!permutations_invariant()) {
		throw InvalidState("Permutations invariant failed");
	}
	if (!corners_invariant()) {
		throw InvalidState("Corners invariant failed");
	}
	if (!edges_invariant()) {
		throw InvalidState("Edges invariant failed");
	}
}

bool CubeValidator::correct_centers() const {
	using namespace RubiksCubeHelper;
	return (cube_.face(DOWN) == opposite_colours[cube_.face(UP)] && cube_.face(LEFT) == opposite_colours[cube_.face(RIGHT)]
		&& cube_.face(BACK) == opposite_colours[cube_.face(FRONT)]);
}

bool CubeValidator::correct_count() const {
	std::vector<uint8_t> stickers_cnt(COLOURS_COUNT);
	using namespace RubiksCubeHelper;
	for (uint8_t i = 0; i < EDGES_COUNT; ++i) {
		for (size_t j = 0; j < 2; ++j) {

			stickers_cnt[index(cube_.edge(static_cast<EdgesIndexing>(i))[j])] += 1;
		}
	}
	for (size_t i = 0; i < CORNERS_COUNT; ++i) {
		for (size_t j = 0; j < 3; ++j) {
			stickers_cnt[index(cube_.corner(static_cast<CornersIndexing>(i))[j])] += 1;
		}
	}
	return std::all_of(stickers_cnt.begin(), stickers_cnt.end(), [](uint8_t num) {return num == 8; });
}

bool CubeValidator::corners_invariant() const {
	using namespace RubiksCubeHelper;
	uint8_t twist_cnt = 0;
	Position::Positions up_col = cube_.face(UP), down_col = opposite_colours[up_col];
	for (uint8_t i = 0; i < CORNERS_COUNT; ++i) {
		if (cube_.corner(static_cast<CornersIndexing>(i))[0] == up_col
			|| cube_.corner(static_cast<CornersIndexing>(i))[0] == down_col) {
			continue;
		}
		if (cube_.corner(static_cast<CornersIndexing>(i))[1] == up_col
			|| cube_.corner(static_cast<CornersIndexing>(i))[1] == down_col) {
			if (i == UP_BACK_LEFT || i == UP_FRONT_RIGHT || i == DOWN_FRONT_LEFT || i == DOWN_BACK_RIGHT) {
				twist_cnt += 1;
			}
			else {
				twist_cnt += 2;
			}
		}
		else {
			if (i == UP_FRONT_LEFT || i == UP_BACK_RIGHT || i == DOWN_FRONT_RIGHT || i == DOWN_BACK_LEFT) {
				twist_cnt += 1;
			}
			else {
				twist_cnt += 2;
			}
		}
	}
	return twist_cnt % 3 == 0;
}

bool CubeValidator::edges_invariant() const {
	using namespace RubiksCubeHelper;
	uint8_t good_edges_cnt = 0;
	Position::Positions down_col = cube_.face(DOWN), up_col = opposite_colours[down_col],
		front_col = cube_.face(FRONT), back_col = opposite_colours[front_col];
	for (uint8_t i = UP_BACK; i <= DOWN_FRONT; ++i) {
		if (cube_.edge(static_cast<EdgesIndexing>(i))[0] == up_col
			|| cube_.edge(static_cast<EdgesIndexing>(i))[0] == down_col) {
			++good_edges_cnt;
		}
		else if (cube_.edge(static_cast<EdgesIndexing>(i))[1] != up_col
			&& cube_.edge(static_cast<EdgesIndexing>(i))[1] != down_col
			&& (cube_.edge(static_cast<EdgesIndexing>(i))[0] == front_col
				|| cube_.edge(static_cast<EdgesIndexing>(i))[0] == back_col)) {
			++good_edges_cnt;
		}
	}
	return good_edges_cnt % 2 == 0;
}

bool CubeValidator::permutations_invariant() const {
	std::vector<Colour> centers = cube_.face_colours();
	return (edges_perm(centers) + corners_perm(centers)) % 2 == 0;
}

size_t CubeValidator::edges_perm(std::vector<Colour> const& centers) const {
	using namespace RubiksCubeHelper;
	std::vector<std::vector<Colour>> edges(EDGES_COUNT);
	for (uint8_t i = UP_BACK; i <= DOWN_FRONT; ++i) {
		edges[i] = cube_.edge_colours(static_cast<EdgesIndexing>(i));
	}
	const std::vector<std::vector<Colour>> destination = {
		{centers[UP], centers[BACK]}, {centers[UP], centers[RIGHT]}, {centers[UP], centers[FRONT]}, {centers[UP], centers[LEFT]},
		{centers[FRONT], centers[LEFT]}, {centers[BACK], centers[LEFT]}, {centers[BACK], centers[RIGHT]}, {centers[FRONT], centers[RIGHT]},
		{centers[DOWN], centers[LEFT]}, {centers[DOWN], centers[BACK]}, {centers[DOWN], centers[RIGHT]}, {centers[DOWN], centers[FRONT]}
	};
	std::vector<uint8_t> permutation(EDGES_COUNT);
	for (uint8_t i = 0; i < EDGES_COUNT; ++i) {
		for (uint8_t j = 0; j < EDGES_COUNT; ++j) {
			if (std::is_permutation(edges[i].begin(), edges[i].end(), destination[j].begin())) {
				permutation[j] = i;
				break;
			}
		}
	}
	size_t inversion_count = 0;
	for (uint8_t i = 0; i < EDGES_COUNT - 1; ++i) {
		for (uint8_t j = i + 1; j < EDGES_COUNT; ++j) {
			if (permutation[i] > permutation[j]) {
				++inversion_count;
			}
		}
	}
	return inversion_count;
}

size_t CubeValidator::corners_perm(std::vector<Colour> const& centers) const {
	using namespace RubiksCubeHelper;
	std::vector<std::vector<Colour>> corners(CORNERS_COUNT);
	for (uint8_t i = UP_BACK_LEFT; i <= DOWN_FRONT_RIGHT; ++i) {
		corners[i] = cube_.corner_colours(static_cast<CornersIndexing>(i));
	}
	const std::vector<std::vector<Colour>> destination = {
		{centers[UP], centers[BACK], centers[LEFT]}, {centers[UP], centers[BACK], centers[RIGHT]},
		{centers[UP], centers[FRONT], centers[RIGHT]}, {centers[UP], centers[FRONT], centers[LEFT]},
		{centers[DOWN], centers[FRONT], centers[LEFT]}, {centers[DOWN], centers[BACK], centers[LEFT]},
		{centers[DOWN], centers[BACK], centers[RIGHT]}, {centers[DOWN], centers[FRONT], centers[RIGHT]}
	};
	std::vector<uint8_t> permutation(CORNERS_COUNT);
	for (uint8_t i = 0; i < CORNERS_COUNT; ++i) {
		for (uint8_t j = 0; j < CORNERS_COUNT; ++j) {
			if (std::is_permutation(corners[i].begin(), corners[i].end(), destination[j].begin())) {
				permutation[j] = i;
				break;
			}
		}
	}
	size_t inversion_count = 0;
	for (uint8_t i = 0; i < CORNERS_COUNT - 1; ++i) {
		for (uint8_t j = i + 1; j < CORNERS_COUNT; ++j) {
			if (permutation[i] > permutation[j]) {
				++inversion_count;
			}
		}
	}
	return inversion_count;
}

std::unordered_map<Position::Positions, Position::Positions> CubeValidator::opposite_colours = {
	{Position::Positions::YELLOW, Position::Positions::WHITE}, {Position::Positions::WHITE, Position::Positions::YELLOW},
	{Position::Positions::ORANGE, Position::Positions::RED}, {Position::Positions::RED, Position::Positions::ORANGE},
	{Position::Positions::GREEN, Position::Positions::BLUE}, {Position::Positions::BLUE, Position::Positions::GREEN}
};