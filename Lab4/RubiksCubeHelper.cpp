#include "RubiksCubeHelper.hpp"

std::string RubiksCubeHelper::create_filename() {
	// Перевод текущего времени в строку
	std::time_t t = std::time(0);
	std::tm now;
	localtime_s(&now, &t);
	char buffer[256];
	std::strftime(buffer, 256, "%F_%H-%M-%S", &now);
	return "CUBE_" + std::string(buffer) + ".cube";
}

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