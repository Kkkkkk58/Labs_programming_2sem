#include "Cube.hpp"
#include "CubeExceptions.hpp"
#include "CubeValidator.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <random>

RubiksCube::RubiksCube()
	: center_(), corners_(std::vector<CornerCubie>(CORNERS_COUNT)), edges_(std::vector<EdgeCubie>(EDGES_COUNT)) {
	default_init();
}

RubiksCube::RubiksCube(CenterCubie const& center, std::vector<CornerCubie> const& corners, std::vector<EdgeCubie> const& edges)
	: center_(center), corners_(corners), edges_(edges) {
	try {
		validate();
	}
	catch (std::exception const& e) {
		std::cerr << "Failed to construct the Cube: " << e.what() << "\nCreating the default one\n\n";
		default_init();
	}
}

RubiksCube::RubiksCube(std::string const& colour_map)
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

void RubiksCube::read_state(std::string const& filename, FileInputType const& fit) {
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

void RubiksCube::save_state(std::string filename) {
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

RubiksCube::RubiksCube(RubiksCube const& other) : center_(other.center_), corners_(other.corners_), edges_(other.edges_) {}

void RubiksCube::swap(RubiksCube& other) {
	using std::swap;
	swap(center_, other.center_);
	swap(corners_, other.corners_);
	swap(edges_, other.edges_);
}

RubiksCube::RubiksCube(RubiksCube&& other) noexcept {
	swap(other);
}

RubiksCube& RubiksCube::operator=(RubiksCube const& other) {
	if (this != &other) {
		center_ = other.center_;
		corners_ = other.corners_;
		edges_ = other.edges_;
	}
	return *this;
}

RubiksCube& RubiksCube::operator=(RubiksCube&& other) noexcept {
	swap(other);
	return *this;
}

bool RubiksCube::is_solved() const {
	return up_solved() && left_solved() && front_solved();
}

void RubiksCube::setup(MoveSequence const& moves) {
	for (size_t i = 0; i < moves.size(); ++i) {
		rotate(moves[i]);
	}
}

void RubiksCube::rotate(Move const& move) {
	using namespace RubiksCubeHelper;
	switch (move.direction()) {
	case Move::Direction::U:
		perform_rotation(UP_BACK_LEFT, UP_BACK_RIGHT, UP_FRONT_RIGHT, UP_FRONT_LEFT, move);
		perform_rotation(UP_BACK, UP_RIGHT, UP_FRONT, UP_LEFT, move);
		break;
	case Move::Direction::D:
		perform_rotation(DOWN_FRONT_LEFT, DOWN_FRONT_RIGHT, DOWN_BACK_RIGHT, DOWN_BACK_LEFT, move);
		perform_rotation(DOWN_LEFT, DOWN_FRONT, DOWN_RIGHT, DOWN_BACK, move);
		break;
	case Move::Direction::L:
		perform_rotation(UP_BACK_LEFT, UP_FRONT_LEFT, DOWN_FRONT_LEFT, DOWN_BACK_LEFT, move);
		perform_rotation(UP_LEFT, FRONT_LEFT, DOWN_LEFT, BACK_LEFT, move);
		break;
	case Move::Direction::R:
		perform_rotation(UP_BACK_RIGHT, DOWN_BACK_RIGHT, DOWN_FRONT_RIGHT, UP_FRONT_RIGHT, move);
		perform_rotation(UP_RIGHT, BACK_RIGHT, DOWN_RIGHT, FRONT_RIGHT, move);
		break;
	case Move::Direction::F:
		perform_rotation(UP_FRONT_RIGHT, DOWN_FRONT_RIGHT, DOWN_FRONT_LEFT, UP_FRONT_LEFT, move);
		perform_rotation(UP_FRONT, FRONT_RIGHT, DOWN_FRONT, FRONT_LEFT, move);
		break;
	case Move::Direction::B:
		perform_rotation(UP_BACK_LEFT, DOWN_BACK_LEFT, DOWN_BACK_RIGHT, UP_BACK_RIGHT, move);
		perform_rotation(UP_BACK, BACK_LEFT, DOWN_BACK, BACK_RIGHT, move);
		break;
	case Move::Direction::M:
		perform_rotation(UP_FRONT, DOWN_FRONT, DOWN_BACK, UP_BACK, move);
		perform_rotation(UP, FRONT, DOWN, BACK, move);
		break;
	case Move::Direction::S:
		perform_rotation(UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT, move);
		perform_rotation(UP, RIGHT, DOWN, LEFT, move);
		break;
	case Move::Direction::E:
		perform_rotation(FRONT_RIGHT, BACK_RIGHT, BACK_LEFT, FRONT_LEFT, move);
		perform_rotation(FRONT, RIGHT, BACK, LEFT, move);
		break;
	case Move::Direction::X:
		rotate(Move(Move::Direction::R, move.clockwise(), move.times()));
		rotate(Move(Move::Direction::L, !move.clockwise(), move.times()));
		rotate(Move(Move::Direction::M, !move.clockwise(), move.times()));
		break;
	case Move::Direction::Y:
		rotate(Move(Move::Direction::U, move.clockwise(), move.times()));
		rotate(Move(Move::Direction::E, !move.clockwise(), move.times()));
		rotate(Move(Move::Direction::D, !move.clockwise(), move.times()));
		break;
	case Move::Direction::Z:
		rotate(Move(Move::Direction::F, move.clockwise(), move.times()));
		rotate(Move(Move::Direction::S, move.clockwise(), move.times()));
		rotate(Move(Move::Direction::B, !move.clockwise(), move.times()));
		break;
	default: break;
	}
}

std::string RubiksCube::sweep() const {
	std::string sweep;
	using namespace RubiksCubeHelper;
	std::vector<Position::Positions> colours_sequence = {
			corners_[UP_BACK_LEFT][0], edges_[UP_BACK][0], corners_[UP_BACK_RIGHT][0], edges_[UP_LEFT][0], center_[0],
			edges_[UP_RIGHT][0], corners_[UP_FRONT_LEFT][0], edges_[UP_FRONT][0], corners_[UP_FRONT_RIGHT][0],
			corners_[UP_BACK_LEFT][2], edges_[UP_LEFT][1], corners_[UP_FRONT_LEFT][2], corners_[UP_FRONT_LEFT][1],
			edges_[UP_FRONT][1], corners_[UP_FRONT_RIGHT][1], corners_[UP_FRONT_RIGHT][2], edges_[UP_RIGHT][1],
			corners_[UP_BACK_RIGHT][2], corners_[UP_BACK_RIGHT][1], edges_[UP_BACK][1], corners_[UP_BACK_LEFT][1],
			edges_[BACK_LEFT][1], center_[1], edges_[FRONT_LEFT][1], edges_[FRONT_LEFT][0], center_[2], edges_[FRONT_RIGHT][0],
			edges_[FRONT_RIGHT][1], center_[3], edges_[BACK_RIGHT][1], edges_[BACK_RIGHT][0], center_[4], edges_[BACK_LEFT][0],
			corners_[DOWN_BACK_LEFT][2], edges_[DOWN_LEFT][1], corners_[DOWN_FRONT_LEFT][2], corners_[DOWN_FRONT_LEFT][1],
			edges_[DOWN_FRONT][1], corners_[DOWN_FRONT_RIGHT][1], corners_[DOWN_FRONT_RIGHT][2], edges_[DOWN_RIGHT][1],
			corners_[DOWN_BACK_RIGHT][2], corners_[DOWN_BACK_RIGHT][1], edges_[DOWN_BACK][1], corners_[DOWN_BACK_LEFT][1],
			corners_[DOWN_FRONT_LEFT][0], edges_[DOWN_FRONT][0], corners_[DOWN_FRONT_RIGHT][0], edges_[DOWN_LEFT][0], center_[5],
			edges_[DOWN_RIGHT][0], corners_[DOWN_BACK_LEFT][0], edges_[DOWN_BACK][0], corners_[DOWN_BACK_RIGHT][0]
	};
	std::vector<Colour> raw_colours = to_colours(colours_sequence);
	for (size_t i = 0; i < 3; ++i) {
		sweep.push_back('\t');
		for (size_t j = 0; j < 3; ++j) {
			sweep.push_back(raw_colours[i * 3 + j]);
			sweep.push_back(' ');
		}
		sweep.push_back('\n');
	}
	for (size_t i = 0; i < 3; ++i) {
		for (size_t j = 0; j < 12; ++j) {
			sweep.push_back(raw_colours[9 + i * 12 + j]);
			sweep.push_back(' ');
			if (j % 3 == 2) {
				sweep += "  ";
			}
		}
		sweep.push_back('\n');
	}
	for (size_t i = 0; i < 3; ++i) {
		sweep.push_back('\t');
		for (size_t j = 0; j < 3; ++j) {
			sweep.push_back(raw_colours[45 + i * 3 + j]);
			sweep.push_back(' ');
		}
		sweep.push_back('\n');
	}
	return sweep;
}

Position::Positions RubiksCube::face(RubiksCubeHelper::CentersIndexing i) const {
	return center_[i];
}

std::vector<Colour> RubiksCube::face_colours() const {
	return center_.get_colours();
}

CornerPosition const& RubiksCube::corner(RubiksCubeHelper::CornersIndexing i) const {
	return corners_[i].get_orientation();
}
std::vector<Colour> RubiksCube::corner_colours(RubiksCubeHelper::CornersIndexing i) const {
	return corners_[i].get_colours();
}
EdgePosition const& RubiksCube::edge(RubiksCubeHelper::EdgesIndexing i) const {
	return edges_[i].get_orientation();
}
std::vector<Colour> RubiksCube::edge_colours(RubiksCubeHelper::EdgesIndexing i) const {
	return edges_[i].get_colours();
}

void RubiksCube::default_init() {
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

bool RubiksCube::up_solved() const {
	using namespace RubiksCubeHelper;
	return std::all_of(corners_.begin() + UP_BACK_LEFT, corners_.begin() + DOWN_FRONT_LEFT,
		[&](CornerCubie const& corner) { return corner[0] == center_[0]; })
		&& std::all_of(edges_.begin() + UP_BACK, edges_.begin() + FRONT_LEFT,
			[&](EdgeCubie const& edge) { return edge[0] == center_[0]; });
}

bool RubiksCube::left_solved() const {
	using namespace RubiksCubeHelper;
	Position::Positions left_colour = center_[1];
	return corners_[DOWN_BACK_LEFT][2] == left_colour && corners_[DOWN_FRONT_LEFT][2] == left_colour
		&& corners_[UP_BACK_LEFT][2] == left_colour && corners_[UP_FRONT_LEFT][2] == left_colour
		&& edges_[BACK_LEFT][1] == left_colour && edges_[DOWN_LEFT][1] == left_colour
		&& edges_[FRONT_LEFT][1] == left_colour && edges_[UP_LEFT][1] == left_colour;
}


bool RubiksCube::front_solved() const {
	using namespace RubiksCubeHelper;
	Position::Positions front_colour = center_[2];
	return corners_[DOWN_FRONT_LEFT][1] == front_colour && corners_[DOWN_FRONT_RIGHT][1] == front_colour
		&& corners_[UP_FRONT_LEFT][1] == front_colour && corners_[UP_FRONT_RIGHT][1] == front_colour
		&& edges_[DOWN_FRONT][1] == front_colour && edges_[FRONT_LEFT][0] == front_colour
		&& edges_[FRONT_RIGHT][0] == front_colour && edges_[UP_FRONT][1] == front_colour;
}

void RubiksCube::validate() {
	CubeValidator validator(*this);
	validator.report();
}

std::string RubiksCube::create_filename() const {
	std::time_t t = std::time(0);
	std::tm now;
	localtime_s(&now, &t);
	char buffer[256];
	std::strftime(buffer, 256, "%F_%H-%M-%S", &now);
	return "CUBE_" + std::string(buffer) + ".cube";
}

void RubiksCube::perform_rotation(RubiksCubeHelper::CornersIndexing a, RubiksCubeHelper::CornersIndexing b, \
	RubiksCubeHelper::CornersIndexing c, RubiksCubeHelper::CornersIndexing d, Move const& move) {
	if (move.times() == 2) {
		std::swap(corners_[a], corners_[c]);
		std::swap(corners_[b], corners_[d]);
		return;
	}
	if (move.clockwise()) {
		std::swap(corners_[d], corners_[c]);
		std::swap(corners_[c], corners_[b]);
		std::swap(corners_[b], corners_[a]);
	}
	else {
		std::swap(corners_[a], corners_[b]);
		std::swap(corners_[b], corners_[c]);
		std::swap(corners_[c], corners_[d]);
	}
	corners_[a].rotate(move);
	corners_[b].rotate(move);
	corners_[c].rotate(move);
	corners_[d].rotate(move);
}

void RubiksCube::perform_rotation(RubiksCubeHelper::EdgesIndexing a, RubiksCubeHelper::EdgesIndexing b, \
	RubiksCubeHelper::EdgesIndexing c, RubiksCubeHelper::EdgesIndexing d, Move const& move) {
	if (move.times() == 2) {
		std::swap(edges_[a], edges_[c]);
		std::swap(edges_[b], edges_[d]);
		return;
	}
	if (move.clockwise()) {
		std::swap(edges_[d], edges_[c]);
		std::swap(edges_[c], edges_[b]);
		std::swap(edges_[b], edges_[a]);
	}
	else {
		std::swap(edges_[a], edges_[b]);
		std::swap(edges_[b], edges_[c]);
		std::swap(edges_[c], edges_[d]);
	}
	edges_[a].rotate(move);
	edges_[b].rotate(move);
	edges_[c].rotate(move);
	edges_[d].rotate(move);
}

void RubiksCube::perform_rotation(RubiksCubeHelper::CentersIndexing a, RubiksCubeHelper::CentersIndexing b, \
	RubiksCubeHelper::CentersIndexing c, RubiksCubeHelper::CentersIndexing d, Move const& move) {
	if (move.times() == 2) {
		std::swap(center_[a], center_[c]);
		std::swap(center_[b], center_[d]);
		return;
	}
	if (move.clockwise()) {
		std::swap(center_[d], center_[c]);
		std::swap(center_[c], center_[b]);
		std::swap(center_[b], center_[a]);
	}
	else {
		std::swap(center_[a], center_[b]);
		std::swap(center_[b], center_[c]);
		std::swap(center_[c], center_[d]);
	}
}

std::ostream& operator<<(std::ostream& os, RubiksCube const& cube) {
	std::string sweep_base = cube.sweep();
	for (char c : sweep_base) {
		if (std::isalpha(c)) {
			os << Colour(c);
		}
		else {
			os << c;
		}
	}
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

RubiksCube random_cube() {
	std::random_device device;
	std::mt19937 generator(device());

	size_t steps = generator() % 40;
	RubiksCube cube;
	MoveSequence s;
	for (size_t i = 0; i < steps; ++i) {
		char dir_index = generator() % 12;
		uint8_t times = generator() % 2 + 1;
		bool clockwise = generator() % 2;
		Move m(static_cast<Move::Direction>(dir_index), clockwise, times);
		s += m;
		cube.rotate(Move(static_cast<Move::Direction>(dir_index), clockwise, times));
	}
	std::cout << s.to_string() << "\n";
	return cube;
}