#ifndef SOLUTION_HPP
#define SOLUTION_HPP
#include "Moves.hpp"


class ISolution {
public:
	virtual ~ISolution() {}
	virtual void solve() = 0;
	virtual MoveSequence sequence() const = 0;
};


#endif
