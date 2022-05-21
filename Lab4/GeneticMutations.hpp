#ifndef GENETIC_MUTATIONS_HPP
#define GENETIC_MUTATIONS_HPP
#include <vector>
#include "Moves.hpp"

// Основные последовательности ходов, используемые в качестве мутаций в генетическом алгоритме
const std::vector<MoveSequence> mutations_sequences = {
	MoveSequence("F' L' B' R' U' R U' B L F R U R' U"),
	MoveSequence("F R B L U L' U B' R' F' L' U' L U'"),
	MoveSequence("U2 B U2 B' R2 F R' F' U2 F' U2 F R'"),
	MoveSequence("U2 R U2 R' F2 L F' L' U2 L' U2 L F'"),
	MoveSequence("U' B2 D2 L' F2 D2 B2 R' U'"),
	MoveSequence("U B2 D2 R F2 D2 B2 L U"),
	MoveSequence("D' R' D R2 U' R B2 L U' L' B2 U R2"),
	MoveSequence("D L D' L2 U L' B2 R' U R B2 U' L2"),
	MoveSequence("R' U L' U2 R U' L R' U L' U2 R U' L U'"),
	MoveSequence("L U' R U2 L' U R' L U' R U2 L' U R' U"),
	MoveSequence("F' U B U' F U B' U'"),
	MoveSequence("F U' B' U F' U' B U"),
	MoveSequence("L' U2 L R' F2 R"),
	MoveSequence("R' U2 R L' B2 L"),
	MoveSequence("M2 U M2 U2 M2 U M2"),
	MoveSequence("R2 F2 R2 F2 R2 F2"),
	MoveSequence("X"), MoveSequence("X'"), MoveSequence("X2"),
	MoveSequence("Y"), MoveSequence("Y2"), MoveSequence("Y'"),
	MoveSequence("Z"), MoveSequence("Z'"), MoveSequence("Z2"),
	MoveSequence("R2 U' F2 L2 B2 D B2 L2 F2 U2 R2"),
	MoveSequence("R2 F R2 U2 L2 B U2 R2 F2 L2 D2 R2 B2 U2"),
	MoveSequence("U R L' F2 R L' U2 D' L2 F2 U' L2 B2 L2 U' F2"),
	MoveSequence("U2 R' U2 F2 U2 R U D' F2 R2 B2 L2 U' D' B2 R2"),
	MoveSequence("U R' L U2 F2 R' L' D' F2 U' R2 U R2 U2 B2 U' L2")
};


#endif
