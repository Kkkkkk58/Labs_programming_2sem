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
#include <iostream>
#include <fstream>
#include <sstream>
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
	RubiksCube()
		: center_(), corners_(std::vector<CornerCubie>(CORNERS_COUNT)), edges_(std::vector<EdgeCubie>(EDGES_COUNT)) {
		default_init();
	}
	RubiksCube(CenterCubie const& center, std::vector<CornerCubie> const& corners, std::vector<EdgeCubie> const& edges)
		: center_(center), corners_(corners), edges_(edges) {
		try {
			validate();
		}
		catch (std::exception const& e) {
			std::cerr << "Failed to construct the Cube: " << e.what() << "\nCreating the default one";
			default_init();
		}
	}
	explicit RubiksCube(std::string const& colour_map)
		: center_(), corners_(std::vector<CornerCubie>(CORNERS_COUNT)), edges_(std::vector<EdgeCubie>(EDGES_COUNT)) {
		try {
			std::vector<Position::Positions> colours;
			RubiksCubeHelper::to_positions(colour_map, colours);
			using CC = CornerCubie;
			using EC = EdgeCubie;
			using CP = CornerPosition;
			using EP = EdgePosition;
			center_ = CenterCubie({ colours[4], colours[22], colours[25], colours[28], colours[31], colours[49] });
			corners_ = { CC(CP(colours[0], colours[20], colours[9])), CC(CP(colours[2], colours[18], colours[17])),
						 CC(CP(colours[8], colours[14], colours[15])), CC(CP(colours[6], colours[12], colours[11])),
						 CC(CP(colours[45], colours[36], colours[35])), CC(CP(colours[51], colours[44], colours[33])),
						 CC(CP(colours[53], colours[42], colours[41])), CC(CP(colours[47], colours[38], colours[39])) };
			edges_ = { EC(EP(colours[1], colours[19])), EC(EP(colours[5], colours[16])), EC(EP(colours[7], colours[13])),
					   EC(EP(colours[3], colours[10])), EC(EP(colours[24], colours[23])), EC(EP(colours[32], colours[21])),
					   EC(EP(colours[30], colours[29])), EC(EP(colours[26], colours[27])), EC(EP(colours[48], colours[34])),
					   EC(EP(colours[52], colours[43])), EC(EP(colours[50], colours[40])), EC(EP(colours[46], colours[37])) };
			validate();
		}
		catch (std::exception const& e) {
			std::cerr << "Failed to construct the Cube: " << e.what() << "\nCreating the default one";
			default_init();
		}
	}
	enum class FileInputType : bool {
		SWEEP,
		SCRAMBLE
	};
	void read_state(std::string const& filename, FileInputType const& fit = FileInputType::SWEEP) {
		std::ifstream fin(filename);
		if (!fin.is_open()) {
			throw InvalidFile(filename);
		}
		std::stringstream file_content;
		file_content << fin.rdbuf();
		if (fit == FileInputType::SWEEP) {
			*this = RubiksCube(file_content.str());
		}
		else {
			setup(file_content.str());
		}
	}

	void save_state(std::string filename = std::string()) {
		if (filename.empty()) {
			filename = create_filename();
		}
		std::ofstream fout(filename);
		if (fout.is_open()) {
			fout << sweep();
		}
		else {
			throw InvalidFile(filename);
		}
	}
		RubiksCube(RubiksCube const& other) : center_(other.center_), corners_(other.corners_), edges_(other.edges_) {}
		void swap(RubiksCube& other) {
			using std::swap;
			swap(center_, other.center_);
			swap(corners_, other.corners_);
			swap(edges_, other.edges_);
		}
		RubiksCube(RubiksCube&& other) noexcept {
			swap(other);
		}
		RubiksCube& operator=(RubiksCube const& other) {
			if (this != &other) {
				center_ = other.center_;
				corners_ = other.corners_;
				edges_ = other.edges_;
			}
			return *this;
		}
		RubiksCube& operator=(RubiksCube&& other) noexcept {
			swap(other);
			return *this;
		}
		~RubiksCube() = default;
		bool is_solved() const {
			return up_solved() && left_solved() && front_solved();
		}
		void setup(MoveSequence const& moves) {
			for (size_t i = 0; i < moves.size(); ++i) {
				rotate(moves[i]);
			}
		}
		void rotate(Move const& move) {

		}
		std::string sweep() const {
			std::string sweep;

			return sweep;
		}

private:
	CenterCubie center_;
	std::vector<CornerCubie> corners_;
	std::vector<EdgeCubie> edges_;

	void default_init() {
		using P = Position::Positions;
		using CP = CornerPosition;
		using EP = EdgePosition;
		using CC = CornerCubie;
		using EC = EdgeCubie;
		center_ = CenterCubie();
		corners_ = { CC(CP(P::WHITE, P::BLUE, P::ORANGE)), CC(CP(P::WHITE, P::BLUE, P::RED)), CC(CP(P::WHITE, P::GREEN, P::RED)),
					CC(CP(P::WHITE, P::GREEN, P::ORANGE)), CC(CP(P::YELLOW, P::GREEN, P::ORANGE)), CC(CP(P::YELLOW, P::BLUE, P::ORANGE)),
					CC(CP(P::YELLOW, P::BLUE, P::RED)), CC(CP(P::YELLOW, P::GREEN, P::RED)) };
		edges_ = { EC(EP(P::WHITE, P::BLUE)), EC(EP(P::WHITE, P::RED)), EC(EP(P::WHITE, P::GREEN)), EC(EP(P::WHITE, P::ORANGE)),
				   EC(EP(P::GREEN, P::ORANGE)), EC(EP(P::BLUE, P::ORANGE)), EC(EP(P::BLUE, P::RED)), EC(EP(P::GREEN, P::RED)),
				   EC(EP(P::YELLOW, P::ORANGE)), EC(EP(P::YELLOW, P::BLUE)), EC(EP(P::YELLOW, P::RED)), EC(EP(P::YELLOW, P::GREEN)) };
	}


	bool up_solved() const {
		using namespace RubiksCubeHelper;
		return std::all_of(corners_.begin() + UP_BACK_LEFT, corners_.begin() + DOWN_FRONT_LEFT,
			[&](CornerCubie const& corner) { return corner[0] == center_[0]; })
			&& std::all_of(edges_.begin() + UP_BACK, edges_.begin() + FRONT_LEFT,
				[&](EdgeCubie const& edge) { return edge[0] == center_[0]; });
	}

	bool left_solved() const {
		using namespace RubiksCubeHelper;
		Position::Positions left_colour = center_[1];
		return corners_[DOWN_BACK_LEFT][2] == left_colour && corners_[DOWN_FRONT_LEFT][2] == left_colour
			&& corners_[UP_BACK_LEFT][2] == left_colour && corners_[UP_FRONT_LEFT][2] == left_colour
			&& edges_[BACK_LEFT][1] == left_colour && edges_[DOWN_LEFT][1] == left_colour
			&& edges_[FRONT_LEFT][1] == left_colour && edges_[UP_LEFT][1] == left_colour;
	}
	bool front_solved() const {
		using namespace RubiksCubeHelper;
		Position::Positions front_colour = center_[2];
		return corners_[DOWN_FRONT_LEFT][1] == front_colour && corners_[DOWN_FRONT_RIGHT][1] == front_colour
			&& corners_[UP_FRONT_LEFT][1] == front_colour && corners_[UP_FRONT_RIGHT][1] == front_colour
			&& edges_[DOWN_FRONT][1] == front_colour && edges_[FRONT_LEFT][0] == front_colour
			&& edges_[FRONT_RIGHT][0] == front_colour && edges_[UP_FRONT][1] == front_colour;
	}
	//void validate(CubeValidator::StickersNumberCheck const& option = CubeValidator::StickersNumberCheck::ENABLED) {
	//	//CubeValidator validator(*this);
	//	//std::string check_results = validator.report(option);
	//	//if (check_results != "OK") {
	//	//	throw InvalidState(check_results);
	//	//}
	//}
	void validate() {
		return;
	}
	std::string create_filename() const {
		std::time_t t = std::time(0);
		std::tm* now;
		localtime_s(now, &t);
		char buffer[256];
		std::strftime(buffer, 256, "F::T", now);
		return "CUBE_" + std::string(buffer) + ".cube";
	}
};

std::ostream& operator<<(std::ostream& os, RubiksCube const& cube) {
	os << cube.sweep();
	return os;
}

std::istream& operator>>(std::istream& is, RubiksCube& cube) {
	std::string s;
	while (is.good()) {
		std::string tmp;
		is >> tmp;
		s += tmp;
	}
	cube = RubiksCube(s);
	return is;
}

#endif
