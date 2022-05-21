#include "UserInteraction.hpp"
#include "GeneticAlgorithm.hpp"
#include <io.h>
#include <conio.h>
#include <fcntl.h>


enum ARROWS {
	UP = 72, LEFT = 75, RIGHT = 77, DOWN = 80
};

void UserInteraction::mainloop() const {
	on_greeting();
	bool alive = true;
	while (alive) {
		try {
			std::cout << "Enter the option: \n\n";
			char option = _getch();
			switch (option) {
			case 'h': case 'H':
				on_options();
				break;
			case 'g': case 'G':
				on_random();
				break;
			case 'd': case 'D':
				on_default();
				break;
			case 'r': case 'R':
				on_read();
				break;
			case 'i': case 'I':
				on_input();
				break;
			case 'e': case 'E':
				alive = false;
				break;
			default:
				on_wrong_command();
				break;
			}
		}
		catch (std::exception const& e) {
			std::cout << "\n\nSomething wrong happened while executing the program!\nTry to restart it...\n";
			std::cerr << e.what() << "\n";
			exit(EXIT_FAILURE);
		}
	}
}

void UserInteraction::on_greeting() const {
	int mode = _setmode(_fileno(stdout), _O_U16TEXT);
	std::wcout <<
		L"\n\t\t██████╗░██╗░░░██╗██████╗░██╗██╗░░██╗██╗░██████╗  ░█████╗░██╗░░░██╗██████╗░███████╗\n"
		L"\t\t██╔══██╗██║░░░██║██╔══██╗██║██║░██╔╝╚█║██╔════╝  ██╔══██╗██║░░░██║██╔══██╗██╔════╝\n"
		L"\t\t██████╔╝██║░░░██║██████╦╝██║█████═╝░░╚╝╚█████╗░  ██║░░╚═╝██║░░░██║██████╦╝█████╗░░\n"
		L"\t\t██╔══██╗██║░░░██║██╔══██╗██║██╔═██╗░░░░░╚═══██╗  ██║░░██╗██║░░░██║██╔══██╗██╔══╝░░\n"
		L"\t\t██║░░██║╚██████╔╝██████╦╝██║██║░╚██╗░░░██████╔╝  ╚█████╔╝╚██████╔╝██████╦╝███████╗\n"
		L"\t\t╚═╝░░╚═╝░╚═════╝░╚═════╝░╚═╝╚═╝░░╚═╝░░░╚═════╝░  ░╚════╝░░╚═════╝░╚═════╝░╚══════╝\n"
		<< std::flush;
	mode = _setmode(_fileno(stdout), _O_TEXT);
	std::cout << "\tThis program solves the Rubik's Cube generated randomly or given by user in a form of a scramble or a sweep\n\n"
		"It uses an evolutionary approach underneath\n"
		"which is why the number of moves required to solve the Cube is high\n"
		"However, total time spent on the solution is very low - approximately 50 ms\n\n";
	on_options();
}

void UserInteraction::on_closing() const {
	std::cout << "Shutting down...\n\n";
}

void UserInteraction::on_options() const {
	std::cout << "Available options:\n"
		"- Generate random valid cube (G)\n"
		"- Read the state of the cube from file (given either by a scramble or by a sweep) (R)\n"
		"   ** It also checks whether the state is correct or not according to the Fundamental Theorem of Cubology **\n"
		"- Take the cube's state from input (I)\n"
		"- Save current state in a file\n"
		"- Generate default cube (D)\n"
		"- Perform rotations of the cube\n"
		"- Show the actual state in a console\n"
		"- Find the solution for any correct state of the cube\n"
		"- Show this menu (H)\n\n";
}

void UserInteraction::on_random() const {
	RubiksCube cube = random_cube();
	on_cube_interaction(cube);
}

void UserInteraction::on_default() const {
	RubiksCube cube;
	on_cube_interaction(cube);
}

void UserInteraction::on_read() const {
	RubiksCube cube;
	std::cout << "Enter the filename: ";
	std::string filename;
	std::cin >> filename;
	std::cout << "Is the cube given as a sweep (<-) or a scramble (->)?\n";
	char ch = _getch();
	ch = _getch();
	switch (ch) {
	case ARROWS::LEFT:
		cube.read_state(filename, RubiksCube::FileInputType::SWEEP);
		break;
	case ARROWS::RIGHT:
		cube.read_state(filename, RubiksCube::FileInputType::SCRAMBLE);
		break;
	default:
		on_wrong_command();
		return;
	}
	on_cube_interaction(cube);
}

void UserInteraction::on_input() const {
	std::cout << "Enter the sweep of the cube: ";
	RubiksCube cube;
	std::cin >> cube;
	std::cout << "\n";
	on_cube_interaction(cube);
}

void UserInteraction::on_save(RubiksCube const& cube, std::string const& filename) const {
	std::cout << "Saving current state in the file named " << cube.save_state(filename) << "\nDone!\n";
}

void UserInteraction::on_cube_interaction(RubiksCube& cube) const {
	std::cout << "\nInitial state of the cube is \n\n" << cube << "\n";
	bool alive = true;
	while (alive) {
		std::cout << "What to do with this cube?\n"
			"   Perform rotations  - P\n"
			"   Save current state - S\n"
			"   Solve              - X\n";
		char option = _getch();
		std::string filename = "";
		std::string move_sequence;
		char file_ch;
		switch (option) {
		case 'x': case 'X':
			on_solve(cube);
			break;
		case 's': case 'S':
			std::cout << "Enter the filename (<-) | Generate it (->)\n";
			file_ch = _getch();
			file_ch = _getch();
			if (file_ch == ARROWS::LEFT) {
				std::cout << "Filename: ";
				std::cin >> filename;
				std::cout << "\n";
			}
			on_save(cube, filename);
			break;
		case 'p': case 'P':
			std::cout << "Enter the sequence of moves to perform: ";
			std::getline(std::cin, move_sequence);
			cube.setup(move_sequence);
			std::cout << cube << "\n";
			break;
		case 'e': case 'E':
			return;
		default:
			on_wrong_command();
			return;
		}
	}
}

void UserInteraction::on_wrong_command() const {
	std::cout << "There is no such command... Try again!\n\n";
	on_options();
}

void UserInteraction::on_solve(RubiksCube& cube) const {
	GeneticAlgorithm solution(cube);
	solution.solve();
	cube.setup(solution.sequence());
	std::cout << "\nPrint the sequence? Y/N\n";
	char ch = _getch();
	if (ch == 'Y' || ch == 'y') {
		std::cout << "The sequence is: " << solution.sequence() << "\n" << cube << "\n\n";
	}
}