#ifndef USER_INTERACTION_HPP
#define USER_INTERACTION_HPP
#include <atomic>
#include "Currency.hpp"
#include "Database.hpp"
#include <set>
#include <string>

extern std::atomic<bool> keep_going;

// Структура для хранения опций, с которыми исполняется программа
struct User_input {
	Currency_type type = Currency_type::ALL;
	std::set<std::string> specific_values{};
	std::pair<bool, std::string> json_enabled{ false, "report" };
};

User_input get_input();
void create_json(Database&, std::string = "report");
#endif
