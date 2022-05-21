#include "UserInteraction.hpp"
#include "GeneticAlgorithm.hpp"
#include <io.h>
#include <conio.h>
#include <fcntl.h>

// Коды символов-"стрелочек" на клавиатуре
enum ARROWS {
	UP = 72, LEFT = 75, RIGHT = 77, DOWN = 80
};

// Главный цикл приложения
void UserInteraction::mainloop() const {
	// Вывод приветственного сообщения
	on_greeting();
	// Начинаем цикл исполнения
	bool alive = true;
	while (alive) {
		try {
			// Получаем опцию с клавиатуры
			std::cout << "Enter the option: \n\n";
			char option = _getch();

			switch (option) {
			// Справка
			case 'h': case 'H':
				on_options();
				break;
			// Генерация случайного куба
			case 'g': case 'G':
				on_random();
				break;
			// Создание обычного куба в собранном состоянии
			case 'd': case 'D':
				on_default();
				break;
			// Чтение куба из файла
			case 'r': case 'R':
				on_read();
				break;
			// Чтение развертки куба из консоли
			case 'i': case 'I':
				on_input();
				break;
			// Завершение основного цикла
			case 'e': case 'E':
				alive = false;
				break;
			// Неизвестная команда
			default:
				on_wrong_command();
				break;
			}
		}
		// Обработка исключений
		catch (std::exception const& e) {
			std::cout << "\n\nSomething wrong happened while executing the program!\nTry to restart it...\n";
			std::cerr << e.what() << "\n";
			exit(EXIT_FAILURE);
		}
	}
}

// Вывод приветственного сообщения
void UserInteraction::on_greeting() const {
	// Смена формата вывода 
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
	// Показ доступных опций
	on_options();
}

// Завершение программы
void UserInteraction::on_closing() const {
	std::cout << "Shutting down...\n\n";
}

// Вывод справки
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

// Генерация случайного куба
void UserInteraction::on_random() const {
	RubiksCube cube = random_cube();
	on_cube_interaction(cube);
}

// Генерация стандартного куба в собранном состоянии
void UserInteraction::on_default() const {
	RubiksCube cube;
	on_cube_interaction(cube);
}

// Чтение куба из файла
void UserInteraction::on_read() const {
	RubiksCube cube;
	std::cout << "Enter the filename: ";
	std::string filename;
	std::cin >> filename;
	std::cout << "Is the cube given as a sweep (<-) or a scramble (->)?\n";
	char ch = _getch();
	ch = _getch();
	switch (ch) {
	// Чтение куба в формате развертки
	case ARROWS::LEFT:
		cube.read_state(filename, RubiksCube::FileInputType::SWEEP);
		break;
	// Чтение куба в формате скрембла
	case ARROWS::RIGHT:
		cube.read_state(filename, RubiksCube::FileInputType::SCRAMBLE);
		break;
	default:
		on_wrong_command();
		return;
	}
	on_cube_interaction(cube);
}

// Чтение состояния куба из консоли
void UserInteraction::on_input() const {
	std::cout << "Enter the sweep of the cube: ";
	RubiksCube cube;
	std::cin >> cube;
	std::cout << "\n";
	on_cube_interaction(cube);
}

// Сохранение текущего состояния в файл
void UserInteraction::on_save(RubiksCube const& cube, std::string const& filename) const {
	std::cout << "Saving current state in the file named " << cube.save_state(filename) << "\nDone!\n";
}

// Цикл взаимодействия с конкретным кубом
void UserInteraction::on_cube_interaction(RubiksCube& cube) const {
	// Печатаем исходную развертку
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
		// Собираем кубик
		case 'x': case 'X':
			on_solve(cube);
			break;
		// Сохраняем текущее состояние в файл
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
		// Исполнение последовательности действий
		case 'p': case 'P':
			std::cout << "Enter the sequence of moves to perform: ";
			std::getline(std::cin, move_sequence);
			cube.setup(move_sequence);
			std::cout << cube << "\n";
			break;
		// Выход из режима взаимодействия с кубом
		case 'e': case 'E':
			return;
		// Нераспознанная команда
		default:
			on_wrong_command();
			return;
		}
	}
}

// Уведомление о неверно введенной команде
void UserInteraction::on_wrong_command() const {
	std::cout << "There is no such command... Try again!\n\n";
	on_options();
}

// Сборка кубика
void UserInteraction::on_solve(RubiksCube& cube) const {
	GeneticAlgorithm solution(cube);
	solution.solve();
	cube.setup(solution.sequence());
	
	std::cout << "\nPrint the sequence? Y/N\n";
	char ch = _getch();
	// Распечатка последовательности ходов
	if (ch == 'Y' || ch == 'y') {
		std::cout << "The sequence is: " << solution.sequence() << "\n" << cube << "\n\n";
	}
}