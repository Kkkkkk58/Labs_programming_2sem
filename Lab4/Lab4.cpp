#include <iostream>
#include <algorithm>
#include <ctime>
#include "Cube.hpp"
#include "GeneticAlgorithm.hpp"
#include "UserInteraction.hpp"
#include <cassert>


int main() {
	UserInteraction program;
	program.mainloop();
	return 0;
}

//int main(int argc, char* argv[]) {
//
//	//RubiksCube cube;
//	//std::cin >> cube;
//	//assert(cube.is_solved());
//	RubiksCube cube = random_cube();
//
//	//std::cout << cube << "\n\n";
//	//cube.read_state("Init.cube");
//	//std::cout << cube.is_solved() << "\n";
//	std::cout << cube << "\n\n\n";
//	//for (size_t i = 0; i < 1000; ++i) {
//	//cube.setup(MoveSequence("B2 D2 U' R' F2 U D' R' L' F' D2 R2 B2 F L2 F2 U' L' F R L2 U' L' D B F U L2 B U'"));
//	//	cube.setup(MoveSequence("R D' R2 U' D R2 U2 F R2 L' F U2 F2 U2 F2 U' F2 D' B2 U'"));
//	//}
//
//	GeneticAlgorithm solver(cube);
//	solver.solve();
//
//	//std::cout << cube.is_solved() << "\n";
//	//std::cout << cube;
//	//cube.save_state();
//	//
//	//std::cout << cube.is_solved() << "\n";
//	std::cout << solver.sequence() << " \n\n\n";
//	cube.setup(solver.sequence());
//	std::cout << cube;
//	
//
//
//	return 0;
//}

