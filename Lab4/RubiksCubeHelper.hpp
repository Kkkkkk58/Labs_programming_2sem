#ifndef RUBIKS_CUBE_HELPER_HPP
#define RUBIKS_CUBE_HELPER_HPP
#include <cinttypes>
#include <vector>
#include "Moves.hpp"

namespace RubiksCubeHelper {

	//
	//namespace CubieHelper {
	//	std::vector<Colour> to_colours(std::string const& colour_map) {
	//		try {
	//			std::vector<uint8_t> number_check(COLOURS_COUNT);
	//			std::vector<Colour> colours(colour_map.size());
	//			size_t i = 0;
	//			for (char symbol : colour_map) {
	//				colours[i] = Colour(symbol);
	//				uint8_t colour_index = static_cast<uint8_t>((colours[i].get_state())[0]);
	//				if (number_check[colour_index] >= 9) {
	//					throw InvalidState("Too many stickers of the same colour");
	//				}
	//				number_check[colour_index] += 1;
	//			}
	//			return colours;
	//		}
	//		catch (std::exception const& e) {
	//			std::cerr << "Failed to recognize the colour map\n" << e.what();
	//			return std::vector<Colour>();
	//		}
	//	}
	//}
	//

	//	RubiksCube generate_cube() {
//		RubiksCube cube;
//
//		return cube;
//	}
//}
	enum Indexing : uint8_t {
		CENTER,
		UP_BACK_LEFT, UP_BACK, UP_BACK_RIGHT, UP_RIGHT, UP_FRONT_RIGHT, UP_FRONT, UP_FRONT_LEFT, UP_LEFT,
		FRONT_LEFT, BACK_LEFT, BACK_RIGHT, FRONT_RIGHT,
		DOWN_FRONT_LEFT, DOWN_LEFT, DOWN_BACK_LEFT, DOWN_BACK, DOWN_BACK_RIGHT, DOWN_RIGHT, DOWN_FRONT_RIGHT, DOWN_FRONT
	};

	std::map<Indexing, std::vector<size_t>> sweep_bijection = {
		{CENTER, {}},
		{UP_BACK_LEFT, {}}, {UP_BACK_RIGHT, {}}, {UP_FRONT_RIGHT, {}}, {UP_FRONT_LEFT, {}},
		{DOWN_FRONT_LEFT, {}}, {DOWN_BACK_LEFT, {}}, {DOWN_BACK_RIGHT, {}}, {DOWN_FRONT_RIGHT, {}},
		{UP_LEFT, {}}, {UP_BACK, {}}, {UP_RIGHT, {}}, {UP_FRONT, {}},
		{FRONT_LEFT, {}}, {BACK_LEFT, {}}, {BACK_RIGHT, {}}, {FRONT_RIGHT, {}},
		{DOWN_LEFT, {}}, {DOWN_BACK, {}}, {DOWN_RIGHT, {}}, {DOWN_FRONT, {}}
	};

	enum class Rotation_type : uint8_t {
		U, U2, UPRIME, UPRIME2,
		D, D2, DPRIME, DPRIME2,
		L, L2, LPRIME, LPRIME2,
		R, R2, RPRIME, RPRIME2,
		F, F2, FPRIME, FPRIME2,
		B, B2, BPRIME, BPRIME2, 
		S, S2, SPRIME, SPRIME2, 
		M, M2, MPRIME, MPRIME2, 
		E, E2, EPRIME, EPRIME2, 
		X, X2, XPRIME, XPRIME2, 
		Y, Y2, YPRIME, YPRIME2, 
		Z, Z2, ZPRIME, ZPRIME2
	};

	std::vector<Indexing> corner_rotation_map = {

		{UP_BACK_RIGHT, UP_FRONT_RIGHT, UP_FRONT_LEFT, UP_BACK_LEFT},
		{}

	};

	std::vector<Indexing> edge_rotation_map = {

	};

}


#endif