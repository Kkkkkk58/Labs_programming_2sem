#ifndef CUBE_HPP
#define CUBE_HPP
#include <cinttypes>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <regex>
#include <bit>
#include <bitset>
#include <map>
#include <unordered_map>
#include "CubeExceptions.hpp"
#include "Position.hpp"
#include "Constants.hpp"
#include "Colour.hpp"
#include "Cubie.hpp"
#include "RubiksCubeHelper.hpp"
#include "CubeValidator.hpp"

class RubiksCube {
public:
	RubiksCube() : cubies_(std::vector<std::shared_ptr<Cubie>>(STICKERS_COUNT)) {
		default_init();
	}
	RubiksCube(std::vector<std::shared_ptr<Cubie>> const& cubies) : cubies_(cubies) {
		try {
			validate();
		}
		catch (std::exception const& e) {
			std::cerr << "Failed to construct the Cube: " << e.what() << "\nCreating the default one";
			default_init();
		}
	}

	//	explicit RubiksCube(std::string colour_map)
	//		: centers_(std::vector<CenterCubie>(FACES_COUNT)), 
	//		edges_(std::vector<EdgeCubie>(EDGES_COUNT)), 
	//		corners_(std::vector<CornerCubie>(CORNERS_COUNT)) {
	//		try {
	//			colour_map.erase(std::remove_if(colour_map.begin(), colour_map.end(), std::isspace), colour_map.end());
	//			std::vector<Colour> colours = CubieHelper::to_colours(colour_map);
	//			if (colours.size() == 0) {
	//				default_init();
	//				return;
	//			}
	//			init_center(colours);
	//			// TODO: Multithreading
	//			init_edges(colours);
	//			init_corners(colours);
	//			validate(CubeValidator::StickersNumberCheck::DISABLED);
	//
	//		}
	//		catch (std::exception const& e) {
	//			std::cerr << "Failed to construct custom Cube:\n" << e.what() << "\nCreating the default one\n";
	//			default_init();
	//			return;
	//		}
	//		
	//	}
		RubiksCube(RubiksCube const& other) : cubies_(other.cubies_) {}
		void swap(RubiksCube& other) {
			using std::swap;
			swap(cubies_, other.cubies_);
		}
		RubiksCube(RubiksCube&& other) noexcept {
			swap(other);
		}
		RubiksCube& operator=(RubiksCube const& other) {
			if (this != &other) {
				cubies_ = other.cubies_;
			}
			return *this;
		}
		RubiksCube& operator=(RubiksCube&& other) noexcept {
			swap(other);
			return *this;
		}
		~RubiksCube() = default;
		bool is_solved() const {
			
		}
	//
	//	void rotate(std::string const& scramble) {
	//		// TODO : Aho-Corasick ??
	//		std::regex command_template("([UuDdLlRrFfBbxXyYzZeEsSmM])([1-9]+)?(')?");
	//		std::sregex_iterator rit(scramble.begin(), scramble.end(), command_template);
	//		std::sregex_iterator rend;
	//	
	//		while (rit != rend) {
	//			char direction = std::string((*rit)[1])[0];
	//			uint8_t times = (std::string((*rit)[2]).length()) ? std::stoi((*rit)[2]) : 1;
	//			bool clockwise = (std::string((*rit)[3]).length() == 0) ? true : false;
	//
	//			perform(direction, clockwise, times);
	//			++rit;
	//		}
	//	}
	//private:
	//	std::vector<CenterCubie> centers_;
	//	std::vector<EdgeCubie> edges_;
	//	std::vector<CornerCubie> corners_;
	//
	//	void default_init() {
	//		using C = Colour::Colours;
	//		centers_ = { CenterCubie(C::WHITE), CenterCubie(C::ORANGE), CenterCubie(C::GREEN), \
	//					CenterCubie(C::RED), CenterCubie(C::BLUE), CenterCubie(C::YELLOW) };
	//
	//		edges_ = { EdgeCubie(C::WHITE, C::ORANGE), EdgeCubie(C::WHITE, C::BLUE), EdgeCubie(C::WHITE, C::RED), EdgeCubie(C::WHITE, C::GREEN),
	//					EdgeCubie(C::GREEN, C::ORANGE), EdgeCubie(C::ORANGE, C::BLUE), EdgeCubie(C::BLUE, C::RED), EdgeCubie(C::RED, C::GREEN),
	//					EdgeCubie(C::ORANGE, C::YELLOW), EdgeCubie(C::BLUE, C::YELLOW), EdgeCubie(C::RED, C::YELLOW), EdgeCubie(C::GREEN, C::YELLOW) };
	//
	//		corners_ = { CornerCubie(C::WHITE, C::ORANGE, C::BLUE), CornerCubie(C::WHITE, C::BLUE, C::RED),
	//					CornerCubie(C::WHITE, C::GREEN, C::RED), CornerCubie(C::WHITE, C::GREEN, C::ORANGE),
	//					CornerCubie(C::ORANGE, C::BLUE, C::YELLOW), CornerCubie(C::RED, C::BLUE, C::YELLOW),
	//					CornerCubie(C::RED, C::GREEN, C::YELLOW), CornerCubie(C::GREEN, C::ORANGE, C::YELLOW) };
	//	}
	//
	//
	//	void init_center(std::vector<Colour> const& colours) {
	//		for (size_t i = 4, j = 0; i < 50, j < FACES_COUNT; i += 9, ++j) {
	//			centers_[j] = CenterCubie(colours[i]);
	//		}
	//	}
	//
	//	void init_corners(std::vector<Colour> const& colour) {
	//		std::vector<std::vector<uint8_t>> indexes = { {0, 38, 9}, {2, 29, 36}, {8, 20, 27}, {6, 11, 18},
	//													  {51, 15, 44}, {53, 42, 35}, {47, 33, 26}, {45, 24, 17} };
	//		Colour top_colour = centers_[UP].get_colour(), bottom_colour = centers_[DOWN].get_colour();
	//		
	//		for (uint8_t i = Corners::UP_LEFT_BACK; i <= Corners::DOWN_LEFT_FRONT; ++i) {
	//			
	//			CornerCubie::Orientation orientation;
	//			if (colour[indexes[i][0]] == top_colour || colour[indexes[i][0]] == bottom_colour) {
	//				orientation = CornerCubie::Orientation::CORRECT;
	//			}
	//			else if (colour[indexes[i][1]] == top_colour || colour[indexes[i][1]] == bottom_colour) {
	//				orientation = CornerCubie::Orientation::CLOCKWISE;
	//			}
	//			else {
	//				orientation = CornerCubie::Orientation::COUNTERCLOCKWISE;
	//			}
	//
	//			corners_[i] = CornerCubie(colour[indexes[i][0]], colour[indexes[i][1]], colour[indexes[i][2]], orientation);
	//		}
	//	}
	//
	//	void init_edges(std::vector<Colour> const& colour) {
	//		std::vector<std::vector<uint8_t>> indexes =   { {3, 10}, {1, 37}, {5, 28}, {7, 19},
	//														{14, 21}, {12, 41}, {32, 39}, {30, 23},
	//														{48, 16}, {52, 34}, {50, 34}, {46, 25} };
	//		for (uint8_t i = Edges::UP_LEFT; i <= Edges::DOWN_FRONT; ++i) {
	//			Faces checked_side, opposite_side;
	//			if (i <= Edges::UP_FRONT) {
	//				checked_side = Faces::UP;
	//				opposite_side = Faces::DOWN;
	//			}
	//			else if (i <= Edges::LEFT_BACK) {
	//				checked_side = Faces::LEFT;
	//				opposite_side = Faces::RIGHT;
	//			}
	//			else if (i <= Edges::RIGHT_FRONT) {
	//				checked_side = Faces::RIGHT;
	//				opposite_side = Faces::LEFT;
	//			}
	//			else {
	//				checked_side = Faces::BACK;
	//				opposite_side = Faces::FRONT;
	//			}
	//
	//			EdgeCubie::Orientation orientation = (centers_[checked_side].get_colour() == colour[indexes[i][0]] ||
	//				centers_[opposite_side].get_colour() == colour[indexes[i][0]])\
	//				? EdgeCubie::Orientation::CORRECT : EdgeCubie::Orientation::INVERTED;
	//			edges_[i] = EdgeCubie(colour[indexes[i][0]], colour[indexes[i][1]], orientation);
	//		}
	//	}
	//
	//	void perform(char direction, bool clockwise, uint8_t times) {
	//		switch (direction) {
	//		case 'x': case 'X':
	//			flip_x(clockwise, times);
	//			break;
	//		case 'y': case 'Y':
	//			flip_y(clockwise, times);
	//			break;
	//		case 'z': case 'Z':
	//			flip_z(clockwise, times);
	//			break;
	//		case 'u': case 'U':
	//			break;
	//		case 'd': case 'D':
	//			break;
	//		case 'l': case 'L':
	//			break;
	//		case 'r': case 'R':
	//			break;
	//		case 'f': case 'F':
	//			break;
	//		case 'b': case 'B':
	//			break;
	//
	//		case 'm': case 'M':
	//			break;
	//		case 'e': case 'E':
	//			break;
	//		case 's': case 'S':
	//			break;
	//		}
	//	}
	//	void flip_x(bool clockwise, uint8_t times) {
	//		for (uint8_t i = 0; i < times; ++i) {
	//			if (clockwise) {
	//				centers_ = { centers_[Faces::FRONT], centers_[Faces::LEFT], centers_[Faces::DOWN],
	//							centers_[Faces::RIGHT], centers_[Faces::UP], centers_[Faces::BACK] };
	//
	//			}
	//			else {
	//				centers_ = { centers_[Faces::BACK], centers_[Faces::LEFT], centers_[Faces::UP],
	//							centers_[Faces::RIGHT], centers_[Faces::DOWN], centers_[Faces::FRONT] };
	//			}
	//
	//		}
	//	}
	//	void flip_y(bool clockwise, uint8_t times) {
	//		for (uint8_t i = 0; i < times; ++i) {
	//			if (clockwise) {
	//				std::rotate(centers_.begin() + LEFT, centers_.begin() + BACK, centers_.begin() + DOWN);
	//			}
	//			else {
	//				std::rotate(centers_.begin() + LEFT, centers_.begin() + FRONT, centers_.begin() + DOWN);
	//			}
	//
	//		}
	//	}
	//	void flip_z(bool clockwise, uint8_t times) {
	//		for (uint8_t i = 0; i < times; ++i) {
	//			if (clockwise) {
	//				centers_ = { centers_[Faces::LEFT], centers_[Faces::DOWN], centers_[Faces::FRONT],
	//							centers_[Faces::UP], centers_[Faces::BACK], centers_[Faces::RIGHT] };
	//			}
	//			else {
	//				centers_ = { centers_[Faces::RIGHT], centers_[Faces::UP], centers_[Faces::FRONT],
	//							centers_[Faces::DOWN], centers_[Faces::BACK], centers_[Faces::LEFT] };
	//			}
	//		}
	//	}

private:
	std::vector<std::shared_ptr<Cubie>> cubies_;
	// 0 - center; 1-4 - upCorners; 5-8 - downCorners; 9-12 - upEdges; 13-16 - midEdges; 17-20 - downEdges

	void default_init() {
		using P = Position::Positions;
		using CP = CornerPosition;
		using EP = EdgePosition;
		using CC = CornerCubie;
		using EC = EdgeCubie;
		cubies_ = { std::shared_ptr<Cubie>(new CenterCubie()), std::shared_ptr<Cubie>(new CC(CP(P::UP, P::BACK, P::LEFT))),
			std::shared_ptr<Cubie>(new EC(EP(P::UP, P::BACK))), std::shared_ptr<Cubie>(new CC(CP(P::UP, P::BACK, P::RIGHT))),
			std::shared_ptr<Cubie>(new EC(EP(P::UP, P::RIGHT))), std::shared_ptr<Cubie>(new CC(CP(P::UP, P::FRONT, P::RIGHT))), 
			std::shared_ptr<Cubie>(new EC(EP(P::UP, P::FRONT))), std::shared_ptr<Cubie>(new CC(CP(P::UP, P::FRONT, P::LEFT))),
			std::shared_ptr<Cubie>(new EC(EP(P::UP, P::LEFT))), std::shared_ptr<Cubie>(new EC(EP(P::FRONT, P::LEFT))),
			std::shared_ptr<Cubie>(new EC(EP(P::BACK, P::LEFT))), std::shared_ptr<Cubie>(new EC(EP(P::BACK, P::RIGHT))),
			std::shared_ptr<Cubie>(new EC(EP(P::FRONT, P::RIGHT))), std::shared_ptr<Cubie>(new CC(CP(P::DOWN, P::FRONT, P::LEFT))), 
			std::shared_ptr<Cubie>(new EC(EP(P::DOWN, P::LEFT))), std::shared_ptr<Cubie>(new CC(CP(P::DOWN, P::BACK, P::LEFT))),
			std::shared_ptr<Cubie>(new EC(EP(P::DOWN, P::BACK))), std::shared_ptr<Cubie>(new CC(CP(P::DOWN, P::BACK, P::RIGHT))), 
			std::shared_ptr<Cubie>(new EC(EP(P::DOWN, P::RIGHT))), std::shared_ptr<Cubie>(new CC(CP(P::DOWN, P::FRONT, P::RIGHT))),
			std::shared_ptr<Cubie>(new EC(EP(P::DOWN, P::FRONT))) };
	}

	void validate(CubeValidator::StickersNumberCheck const& option = CubeValidator::StickersNumberCheck::ENABLED) {
		CubeValidator validator(*this);
		std::string check_results = validator.report(option);
		if (check_results != "OK") {
			throw InvalidState(check_results);
		}
	}
};

#endif
