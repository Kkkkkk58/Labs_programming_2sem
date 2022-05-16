#ifndef CUBE_VALIDATOR_HPP
#define CUBE_VALIDATOR_HPP
#include <string>


class RubiksCube;

class CubeValidator {
public:
	enum class StickersNumberCheck : bool {
		DISABLED, ENABLED
	};
	CubeValidator(RubiksCube const& cube) : cube_(cube) {}
	std::string report(StickersNumberCheck const& option = StickersNumberCheck::ENABLED) {
		// TODO: Run multithreading
		if (option == StickersNumberCheck::ENABLED && !correct_stickering) {
			return "Incorrect stickering";
		}
		if (!edges_invariant()) {
			return "Edges invariant failed";
		}
		if (!corners_invariant()) {
			return "Corners invariant failed";
		}
		if (!permutations_invariant()) {
			return "Permutations invariant failed";
		}
		return "OK";
	}
private:
	//TODO:
	bool correct_stickering() const {
		return true;
	}
	bool edges_invariant() const {
		return true;
	}
	bool corners_invariant() const {
		return true;
	}
	bool permutations_invariant() const {
		return true;
	}
	RubiksCube cube_;
};
#endif
