#ifndef SOLUTION_HPP
#define SOLUTION_HPP
#include "Cube.hpp"


class ISolution {
public:
	virtual ~ISolution() {}
	virtual void solve() = 0;
};
// TODO
//class BeginnersMethod : public ISolution {
// };
//class Kociemba : public ISolution {
//
//};
//class Friedrich : public ISolution {
//};
//class Korf : public ISolution {
// 
//};
// class Thistlewaite : public ISolution {
// 
// };
// class Pochmann : public ISolution {
// 
// }
//class GeneticAlgorithm : public ISolution {
//
//};

#endif
