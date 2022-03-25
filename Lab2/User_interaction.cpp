#include "User_interaction.hpp"
#include <iostream>
#include <regex>
#include <string>
#include "json/json.hpp"
#include <fstream>

std::atomic<bool> keep_going = true;
User_input get_input() {
	std::string input;
	std::getline(std::cin, input);
	input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
	std::regex primary_expression("--([a-zA-Z]+)=?([a-zA-Z,]+)?");
	std::sregex_iterator rit(input.begin(), input.end(), primary_expression);
	std::sregex_iterator rend;
	User_input options;
	while (rit != rend) {
		std::string key((*rit)[1]);
		std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });
		if (key == "stop") {
			keep_going = false;
		}
		else if (key == "type") {
			std::string type_wanted((*rit)[2]);
			std::transform(type_wanted.begin(), type_wanted.end(), type_wanted.begin(), [](unsigned char c) { return std::tolower(c);  });
			if (type_wanted == "crypto") {
				options.type = Currency_type::CRYPTO;
			}
			else if (type_wanted == "regular") {
				options.type = Currency_type::REGULAR;
			}
			else if (type_wanted == "none") {
				options.type = Currency_type::NONE;
			}
			else {
				options.type = Currency_type::ALL;
			}
		}
		else if (key == "codes") {
			std::string codes((*rit)[2]);
			std::transform(codes.begin(), codes.end(), codes.begin(), [](unsigned char c) { return std::toupper(c); });
			std::regex args("([A-Z]+),?");
			std::sregex_iterator rit_inner(codes.begin(), codes.end(), args);
			std::sregex_iterator rend_inner;
			while (rit_inner != rend_inner) {
				options.specific_values.insert(std::string((*rit_inner)[1]));
				++rit_inner;
			}
		}
		else if (key == "json") {
			options.json_enabled.first = true;
			if (!std::string((*rit)[2]).empty()) {
				options.json_enabled.second = std::string((*rit)[2]);
			}
		}
		++rit;
	}
	return options;
}

void create_json(Database& db, std::string name) {
	if (name.rfind(".json") == std::string::npos) {
		name += ".json";
	}
	std::string sql("SELECT * FROM currencies");
	sqlite3_stmt* stmt;
	int response = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (response != SQLITE_OK) {
		std::cerr << "Problems while making sql query\n";
		return;
	}
	nlohmann::ordered_json result_json;
	int ret_code = sqlite3_step(stmt);
	while (ret_code == SQLITE_ROW) {
		std::string charcode(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		result_json["Currencies"][charcode]["Name"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		result_json["Currencies"][charcode]["Type"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
		result_json["Currencies"][charcode]["Average"] = sqlite3_column_double(stmt, 3);
		result_json["Currencies"][charcode]["Median"] = sqlite3_column_double(stmt, 4);
		for (int i = 5; i < columns.size(); ++i) {
			result_json["Currencies"][charcode]["History"][columns[i - 5]] = sqlite3_column_double(stmt, i);
		}
		ret_code = sqlite3_step(stmt);
	}
	if (ret_code != SQLITE_DONE) {
		std::cerr << "Not all rows are present\n";
	}
	sqlite3_finalize(stmt);
	std::ofstream output(name.c_str());
	output << result_json;
}