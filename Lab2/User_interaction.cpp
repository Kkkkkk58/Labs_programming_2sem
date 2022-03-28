#include "User_interaction.hpp"
#include <iostream>
#include <regex>
#include <string>
#include "json/json.hpp"
#include <fstream>

// Переменная-флаг необходимости продолжения работы
std::atomic<bool> keep_going = true;

// Функция для получения пользовательского ввода с опциями 
User_input get_input() {
	std::string input;
	std::getline(std::cin, input);
	// Стираем пробелы из строки
	input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
	// Регулярное выражение для разбора параметров
	std::regex primary_expression("--([a-zA-Z]+)=?([a-zA-Z,]+)?");
	std::sregex_iterator rit(input.begin(), input.end(), primary_expression);
	std::sregex_iterator rend;
	User_input options;
	// Разбор параметров
	while (rit != rend) {
		std::string key((*rit)[1]);
		// Приведение строки с именем параметра к нижнему регистру для удобной работы
		std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });
		// Если пользователь хочет остановить работу программы
		if (key == "stop") {
			keep_going = false;
		}
		// Опция отображения валют определённого типа
		else if (key == "type") {
			std::string type_wanted((*rit)[2]);
			std::transform(type_wanted.begin(), type_wanted.end(), type_wanted.begin(), [](unsigned char c) { return std::tolower(c);  });
			// Отображение только криптовалюты
			if (type_wanted == "crypto") {
				options.type = Currency_type::CRYPTO;
			}
			// Отображение только обычной валюты
			else if (type_wanted == "regular") {
				options.type = Currency_type::REGULAR;
			}
			// Не отображать таблицу
			else if (type_wanted == "none") {
				options.type = Currency_type::NONE;
			}
			// Показать все валюты
			else {
				options.type = Currency_type::ALL;
			}
		}
		// Опция отображения конкретных валют
		else if (key == "codes") {
			std::string codes((*rit)[2]);
			std::transform(codes.begin(), codes.end(), codes.begin(), [](unsigned char c) { return std::toupper(c); });
			// Разбор параметров, переданных через запятую как коды валют
			std::regex args("([A-Z]+),?");
			std::sregex_iterator rit_inner(codes.begin(), codes.end(), args);
			std::sregex_iterator rend_inner;
			// Добавление кода валюты в множество требуемых кодов
			while (rit_inner != rend_inner) {
				options.specific_values.insert(std::string((*rit_inner)[1]));
				++rit_inner;
			}
		}
		// Опция сохранения данных в json файл
		else if (key == "json") {
			options.json_enabled.first = true;
			// Если было также предоставлено название желаемого файла
			if (!std::string((*rit)[2]).empty()) {
				options.json_enabled.second = std::string((*rit)[2]);
			}
		}
		++rit;
	}
	return options;
}

// Функция записи данных в json файл
void create_json(Database& db, std::string name) {
	// Если название передано без расширения, добавляем ".json"
	if (name.rfind(".json") == std::string::npos) {
		name += ".json";
	}
	// Создание SQL-запроса на получение доступа к базе данных
	std::string sql("SELECT * FROM currencies");
	sqlite3_stmt* stmt;
	int response = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (response != SQLITE_OK) {
		std::cerr << "Problems while making sql query\n";
		return;
	}
	nlohmann::ordered_json result_json;
	int ret_code = sqlite3_step(stmt);
	// Получаем информацию по каждой валюте и записываем в json
	while (ret_code == SQLITE_ROW) {
		std::string char_code(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		result_json["Currencies"][char_code]["Name"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		result_json["Currencies"][char_code]["Type"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
		result_json["Currencies"][char_code]["Average"] = sqlite3_column_double(stmt, 3);
		result_json["Currencies"][char_code]["Median"] = sqlite3_column_double(stmt, 4);
		for (int i = 5; i < columns.size(); ++i) {
			result_json["Currencies"][char_code]["History"][columns[i - 5]] = sqlite3_column_double(stmt, i);
		}
		ret_code = sqlite3_step(stmt);
	}
	if (ret_code != SQLITE_DONE) {
		std::cerr << "Not all rows were accessed\n";
	}
	sqlite3_finalize(stmt);
	std::ofstream output(name.c_str());
	output << result_json;
}
