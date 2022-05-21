#ifndef USER_INTERACTION_HPP
#define USER_INTERACTION_HPP
#include <io.h>
#include <conio.h>
#include <fcntl.h>
#include "Cube.hpp"
#include "GeneticAlgorithm.hpp"

class UserInteraction {
public:
	UserInteraction() {}
	~UserInteraction() { on_closing(); }
	void mainloop() const;
private:
	void on_greeting() const;
	void on_closing() const;
	void on_save(RubiksCube const&, std::string const& = "") const;
	void on_options() const;
	void on_random() const;
	void on_default() const;
	void on_read() const;
	void on_cube_interaction(RubiksCube&) const;
	void on_wrong_command() const;
	void on_solve(RubiksCube& cube) const;
};

#endif
