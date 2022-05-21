#include "GeneticAlgorithm.hpp"
#include "GeneticMutations.hpp"
#include <Windows.h>
#include <algorithm>
#include <random>

// Конструктор класса GeneticAlgorithm от кубика
GeneticAlgorithm::GeneticAlgorithm(RubiksCube const& cube)
	: ISolution(), initial_state_(cube),
	population_({ Candidate(cube) }), found_solution(false) {
	population_[0].fitness();
}

// Метод нахождения решения с помощью генетического алгоритма
void GeneticAlgorithm::solve() {
	// Печатаем в консоль прогресс-бар
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x2);
	std::cout << "\n\n\t\t\t\t\t\t   Calculating...\n\n\t\t\t\t";
	for (size_t i = 0; i < 54; ++i) {
		std::cout << (char)177;
	}
	// Если получен уже собранный кубик
	if (population_[0].fitness() == STICKERS_COUNT) {
		std::cout << "\r\t\t\t\t";
		for (size_t i = 0; i < 54; ++i) {
			std::cout << (char)219;
		}
		SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);
		found_solution = true;
	}
	// Генерируем первую популяцию
	reset();
	// Измеряем время работы
	std::clock_t start;
	double duration;
	start = std::clock();
	// Пока не найдем решинение, ищем
	while (!found_solution) {
		std::cout << "\r\t\t\t\t";
		size_t curr_rate = 0;
		// Пока не достигли терминальной стадии развития
		for (size_t i = 0; i < DEATH_AGE; ++i) {
			for (auto& cube : population_) {
				// Применяем случайные мутации к популяции
				cube.perform_sequence(get_combination());
			}
			// Сортировка представителей популяции для отбора сильнейших
			std::sort(population_.begin(), population_.end(),
				[](Candidate& a, Candidate& b) { return a.fitness() > b.fitness(); });

			// Если нашли решение
			if (population_[0].fitness() == STICKERS_COUNT) {
				for (size_t i = 0; i < population_[0].fitness() - curr_rate; ++i) {
					std::cout << (char)219;
				}
				std::cout << "\n\n\n";
				SetConsoleTextAttribute(console, (WORD)(0 << 4) | 0x7);

				found_solution = true;
				break;
			}
			// Иначе оставляем 15 сильнейших представителей и образуем новую популяцию, опираясь на них
			else {
				if (population_[0].fitness() > curr_rate) {
					for (size_t i = 0; i < population_[0].fitness() - curr_rate; ++i) {
						std::cout << (char)219;
					}
					curr_rate = population_[0].fitness();
				};
			}
			std::copy(population_.begin(), population_.begin() + 16, population_.begin() + 16);
			std::copy(population_.begin(), population_.begin() + 31, population_.begin() + 31);
			std::copy(population_.begin(), population_.begin() + 50, population_.begin() + 50);

		}
		// Если после терминальной стадии ветка эволюции зашла в тупик, генерируем новое поколение от исходного куба
		if (!found_solution) {
			reset();
		}
		else {
			// Сообщаем, сколько времени потрачено на поиск решения
			duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
			std::cout << "\t\t\t\t\t\tTime elapsed: " << duration << "\n\n\n";
			break;
		}
	}
}

// Возврат последовательности, решающей кубик (если решение уже было найдено)
MoveSequence GeneticAlgorithm::sequence() const {
	if (found_solution) {
		return population_[0].get_moves();
	}
	return MoveSequence();
}

// Генерация новой популяции
void GeneticAlgorithm::reset() {
	population_ = std::vector<Candidate>(POPULATION_NUMBER, Candidate(initial_state_));
}

// Получение кода случайной мутации
size_t GeneticAlgorithm::get_combination() const {
	std::random_device device;
	std::mt19937 generator(device());
	return generator() % mutations_sequences.size();
}

// Инициализация статических полей класса
const size_t GeneticAlgorithm::DEATH_AGE = 100, GeneticAlgorithm::POPULATION_NUMBER = 100;