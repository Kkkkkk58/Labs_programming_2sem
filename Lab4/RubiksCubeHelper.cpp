#include "RubiksCubeHelper.hpp"


void RubiksCubeHelper::to_positions(std::string const& colour_map, std::vector<Position::Positions>& colour_buf) {
	for (char c : colour_map) {
		if (std::isalpha(c)) {
			colour_buf.push_back(Colour(c));
		}
	}
}


std::vector<Colour> RubiksCubeHelper::to_colours(std::vector<Position::Positions> const& colour_sequence) {
	std::vector<Colour> colours;
	for (Position::Positions const& pos : colour_sequence) {
		colours.push_back(Colour(pos));
	}
	return colours;
}