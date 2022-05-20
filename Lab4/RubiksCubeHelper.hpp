#ifndef RUBIKS_CUBE_HELPER_HPP
#define RUBIKS_CUBE_HELPER_HPP
#include <cinttypes>
#include <vector>
#include "Colour.hpp"
#include "Position.hpp"

namespace RubiksCubeHelper {

	enum CentersIndexing : uint8_t {
		UP, LEFT, FRONT, RIGHT, BACK, DOWN
	};

 	enum CornersIndexing : uint8_t {
		UP_BACK_LEFT, UP_BACK_RIGHT, UP_FRONT_RIGHT, UP_FRONT_LEFT, 
		DOWN_FRONT_LEFT, DOWN_BACK_LEFT, DOWN_BACK_RIGHT, DOWN_FRONT_RIGHT
	};

	enum EdgesIndexing : uint8_t {
		UP_BACK, UP_RIGHT, UP_FRONT, UP_LEFT, 
		FRONT_LEFT, BACK_LEFT, BACK_RIGHT, FRONT_RIGHT, 
		DOWN_LEFT, DOWN_BACK, DOWN_RIGHT, DOWN_FRONT
	};

	void to_positions(std::string const&, std::vector<Position::Positions>&);

	std::vector<Colour> to_colours(std::vector<Position::Positions> const&);
	
}


#endif