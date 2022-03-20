#define CURL_STATICLIB
#include <iostream>
#include <map>
#include <string>
#include <mutex>
#include <fstream>
#include <vector>
#include <set>
#include <thread>
#include <ctime>
#include <future>
#include <regex>
#include <chrono>
#include "curl/curl.h"
#include "json/json.hpp"
#include "sqlite3/sqlite3.h"
#include "fort/fort.hpp"

std::vector<std::string> columns;
int rows_number;

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

sqlite3* db;
char time_buf[26];
std::chrono::steady_clock::time_point begin;
void open_table() {
	std::cout << "Opening the table..." << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() / 1000000.0 << "\n";
	char* error_message;
	int response = sqlite3_open("CurrenciesDB.db", &db);
	if (response) {
		std::cerr << "Can't open the database: " << sqlite3_errmsg(db);
	}
	std::string sql("CREATE TABLE IF NOT EXISTS currencies (CHARCODE VARCHAR(10) PRIMARY KEY  NOT NULL, NAME NVARCHAR(100) NOT NULL, TYPE VARCHAR(10) NOT NULL, AVERAGE FLOAT NULL, MEDIAN FLOAT NULL);");
	response = sqlite3_exec(db, sql.c_str(), callback, 0, &error_message);
	if (response != SQLITE_OK) {
		std::cerr << "ERR: " << error_message;
		sqlite3_free(error_message);
	}
	sqlite3_stmt* stmt;
	sql = "SELECT COUNT(*) FROM currencies;";
	response = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (response != SQLITE_OK) {
		std::cout << "FUCK U DUMB\n";
	}
	sqlite3_step(stmt);
	rows_number = sqlite3_column_int(stmt, 0);
	//std::cout << rows_number << "\n";
	sqlite3_reset(stmt);
	sql = "SELECT sql FROM sqlite_master WHERE tbl_name = \'currencies\' AND type = \'table\';";
	response = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (response != SQLITE_OK) {
		std::cout << "FUCK U DUMB\n";
	}
	sqlite3_step(stmt);
	std::string col_names = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
	//std::cout << "LOSHARA" << col_names;
	sqlite3_finalize(stmt);
	std::regex temp("\'([a-zA-Z0-9: ]+)\' FLOAT NULL");
	std::sregex_iterator rit(col_names.begin(), col_names.end(), temp);
	std::sregex_iterator rend;
	while (rit != rend) {
		//std::cout << (*rit)[1] << " YESSS Parsed\n";
		columns.push_back((*rit)[1]);
		++rit;
	}
	std::cout << "Table opened" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() << "\n";
}

const std::string USER_AGENT("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36");
enum class Currency_type {
	NONE = -1,
	ALL,
	REGULAR,
	CRYPTO
};
struct Currency_info {
	std::string name = "";
	Currency_type type = Currency_type::ALL;
	double value = 0, avg = 0, median = 0;
};

class Currency_holder {
public:
	Currency_holder(Currency_holder const& other) = delete;
	void operator=(Currency_holder const& other) = delete;
	static Currency_holder* get_instance(std::map<const std::string, Currency_info> const& map);
	static void release_instance();
	std::map<const std::string, Currency_info> const& value() const {
		return currencies_storage_;
	}
	Currency_info value(std::string const& char_code) const {
		if (contains(char_code)) {
			return currencies_storage_.at(char_code);
		}
		else {
			return Currency_info{};
		}
	}
	void add_value(std::string const& char_code, Currency_info const& info) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (currencies_storage_.contains(char_code)) {
			currencies_storage_[char_code] = info;
		}
		else {
			currencies_storage_.emplace(char_code, info);
		}
	}
	size_t size() const {
		return currencies_storage_.size();
	}
	bool contains(std::string const& char_code) const {
		return currencies_storage_.contains(char_code);
	}
	double get_curr_value(std::string const& char_code) const {
		auto finder = currencies_storage_.find(char_code);
		if (finder == currencies_storage_.end()) {
			return 0;
		}
		else {
			return finder->second.value;
		}
	}
private:
	std::map<const std::string, Currency_info> currencies_storage_;
	static Currency_holder* pinstance_;
	Currency_holder(std::map<const std::string, Currency_info> const &map)
		: currencies_storage_(map) {}
	~Currency_holder() = default;
	static std::mutex mutex_;
};

Currency_holder* Currency_holder::pinstance_{ nullptr };
std::mutex Currency_holder::mutex_;
Currency_holder* Currency_holder::get_instance(std::map<const std::string, Currency_info> const& currency_map) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		try {
			pinstance_ = new Currency_holder(currency_map);
		}
		catch (std::exception const& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	return pinstance_;
}
void Currency_holder::release_instance() {
	std::lock_guard<std::mutex> lock(mutex_);
	delete pinstance_;
	pinstance_ = NULL;
}


static size_t write_response(void* ptr, size_t size, size_t nmemb, std::string* data) {
	if (data) {
		data->append(static_cast<char*>(ptr), size * nmemb);
		return size * nmemb;
	}
	else {
		return 0;
	}
}

void regular_currency_parsing(Currency_holder* holder) {
	std::cout << "Regular parsing started..." << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() /1000000.0  << "\n";
	CURL* curl = curl_easy_init();
	if (curl) {
		static char error_buffer[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.cbr-xml-daily.ru/daily_json.js");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
		std::string response_data;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
		CURLcode result = curl_easy_perform(curl);
		if (result != CURLE_OK) {
			std::cerr << "Action failed: " << error_buffer << "\n";
			curl_easy_cleanup(curl);
			return;
		}
		curl_easy_cleanup(curl);
		char* err;
		nlohmann::json parsed_currencies = nlohmann::json::parse(response_data);
		std::string sql = "ALTER TABLE currencies ADD \'" + std::string(time_buf) + "\' FLOAT NULL";
		int request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
		if (request != SQLITE_OK) {
			//std::cerr << "CRINGE: " << err << "\n";
			//sqlite3_free(err);
		}
		columns.push_back(std::string(time_buf));
		for (auto const& item : parsed_currencies["Valute"].items()) {
			std::string char_code(item.key());
			std::string name(item.value()["Name"]);
			//std::cout << "FOUND NAME:" << name << "\n";
			double value(item.value()["Value"]);			
			sql = "INSERT OR IGNORE INTO currencies(CHARCODE, NAME, TYPE) VALUES(\'" + char_code + "\',\'" + name + "\',\'regular\')";
			//std::string sql = "INSERT INTO currencies(CHARCODE,NAME,TYPE) SELE"
			request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
			if (request != SQLITE_OK) {
				std::cerr << "CRINGE: " << err << "\n";
				sqlite3_free(err);
			}
			//sql = "INSERT INTO currencies SELECT \'" + std::string(time_buf) + "\' where CHARCODE = \'" + std::string(iter.first) + "\' VALUES (" + std::to_string(holder->get_curr_value(iter.first)) + ")";
			sql = "UPDATE currencies set \'" + std::string(time_buf) + "\' = " + std::to_string(value) + " where CHARCODE = \'" + char_code + "\'";
			request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
			if (request != SQLITE_OK) {
				std::cerr << "CRINGE: " << err << "\n";
				sqlite3_free(err);
			}
			sql = "SELECT * FROM currencies WHERE CHARCODE = \'" + char_code + "\'";
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
			//std::cout << "COLS" << columns.size();
			std::vector<double> currency_history(columns.size());
			sqlite3_step(stmt);
			for (int i = 0; i < columns.size(); ++i) {
				//std::cout << i << " :" << sqlite3_column_double(stmt, i + 5) << " ;";
				currency_history[i] = sqlite3_column_double(stmt, i + 5);
			}
			double avg = std::accumulate(currency_history.begin(), currency_history.end(), 0.0) / static_cast<double>(columns.size());
			//std::cout << "AVG = " << avg << " SUM = " << avg * columns.size();
			double median;
			if (currency_history.size() % 2 != 0) {
				std::nth_element(currency_history.begin(), currency_history.begin() + currency_history.size() / 2, currency_history.end());
				median = currency_history[currency_history.size() / 2];
			}
			else {
				std::nth_element(currency_history.begin(), currency_history.begin() + currency_history.size() / 2, currency_history.end());
				std::nth_element(currency_history.begin(), currency_history.begin() + (currency_history.size() - 1) / 2, currency_history.end());
				median = (currency_history[(currency_history.size() - 1) / 2] + \
					currency_history[currency_history.size() / 2]) / 2.0;
			}
			holder->add_value(char_code, Currency_info(name, Currency_type::REGULAR, value, avg, median));
			//std::cout << "MED " << median << "\n";
			sql = "UPDATE currencies SET \'AVERAGE\' = " + std::to_string(avg) + ", \'MEDIAN\' = " + std::to_string(median) + " WHERE CHARCODE = \'" + char_code + "\'";
			request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
			if (request != SQLITE_OK) {
				std::cerr << "CRINGE: " << err << "\n";
				sqlite3_free(err);
			}
		}

	}
	std::cout << "Regular parsing done" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() /1000000.0  << "\n";
}

void crypto_currencies_list(std::map<const std::string, std::string> &full_names, bool &status) {
	CURL* curl = curl_easy_init();
	std::cout << "Crypto listing started..." << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() /1000000.0  << "\n";
	if (curl) {
		static char error_buffer[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
		curl_easy_setopt(curl, CURLOPT_URL, "http://api.coinlayer.com/api/list?access_key=514ffac08ad6e991a629f46326d6adda");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
		std::string response;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		CURLcode result = curl_easy_perform(curl);
		if (result != CURLE_OK) {
			std::cerr << "Action failed: " << error_buffer << "\n";
			curl_easy_cleanup(curl);
			return;
		}
		curl_easy_cleanup(curl);
		nlohmann::json parsed_names = nlohmann::json::parse(response);
		for (auto const& item : parsed_names["crypto"].items()) {
			//std::cout << item.key() << " " << item.value()["name"] << " ";
			full_names[item.key()] = (item.value()["name"]);
		}
	}
	status = true;
	std::cout << "Crypto listing done" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() /1000000.0  << "\n";
}


void crypto_currency_parse(Currency_holder* holder, std::future<void> &regular) {
	std::map<const std::string, std::string> currency_names;
	std::cout << "Crypto parsing started..." << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() /1000000.0  << "\n";
	bool status = false;
	std::future<void> get_list = std::async(std::launch::async, crypto_currencies_list, std::ref(currency_names), std::ref(status));
	//bool listing_status = false;
	//std::thread get_list(crypto_currencies_list, std::ref(currency_names), std::ref(listing_status));
	CURL* curl = curl_easy_init();
	if (curl) {
		static char error_buffer[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
		curl_easy_setopt(curl, CURLOPT_URL, "http://api.coinlayer.com/api/live?access_key=514ffac08ad6e991a629f46326d6adda");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
		std::string response;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		CURLcode result = curl_easy_perform(curl);
		if (result != CURLE_OK) {
			std::cerr << "Action failed: " << error_buffer << "\n";
			curl_easy_cleanup(curl);
			return;
		}
		curl_easy_cleanup(curl);
		nlohmann::json parsed_currencies = nlohmann::json::parse(response);
		regular.get();
		const double to_rubles = holder->get_curr_value("USD");
		for (auto const& item : parsed_currencies["rates"].items()) {
			std::string char_code(item.key());
			double value = static_cast<double>(item.value()) * to_rubles;
			char* err;
			if (!holder->contains(char_code) && !status) {
				get_list.get();
			}
			std::string sql = "INSERT OR IGNORE INTO currencies(CHARCODE, NAME, TYPE) VALUES(\'" + char_code + "\',\'" + currency_names[char_code] + "\',\'crypto\')";
			//std::string sql = "INSERT INTO currencies(CHARCODE,NAME,TYPE) SELE"
			int request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
			if (request != SQLITE_OK) {
				std::cerr << "CRINGE: " << err << "\n";
				sqlite3_free(err);
			}
			//sql = "INSERT INTO currencies SELECT \'" + std::string(time_buf) + "\' where CHARCODE = \'" + std::string(iter.first) + "\' VALUES (" + std::to_string(holder->get_curr_value(iter.first)) + ")";
			sql = "UPDATE currencies set \'" + std::string(time_buf) + "\' = " + std::to_string(value) + " where CHARCODE = \'" + char_code + "\'";
			request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
			if (request != SQLITE_OK) {
				std::cerr << "CRINGE: " << err << "\n";
				sqlite3_free(err);
			}
			sql = "SELECT * FROM currencies WHERE CHARCODE = \'" + char_code + "\'";
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
			//std::cout << "COLS" << columns.size();
			std::vector<double> currency_history(columns.size());
			sqlite3_step(stmt);
			for (int i = 0; i < columns.size(); ++i) {
				//std::cout << i << " :" << sqlite3_column_double(stmt, i + 5) << " ;";
				currency_history[i] = sqlite3_column_double(stmt, i + 5);
			}
			double avg = std::accumulate(currency_history.begin(), currency_history.end(), 0.0) / static_cast<double>(columns.size());
			//std::cout << "AVG = " << avg << " SUM = " << avg * columns.size();
			double median;
			if (currency_history.size() % 2 != 0) {
				std::nth_element(currency_history.begin(), currency_history.begin() + currency_history.size() / 2, currency_history.end());
				median = currency_history[currency_history.size() / 2];
			}
			else {
				std::nth_element(currency_history.begin(), currency_history.begin() + currency_history.size() / 2, currency_history.end());
				std::nth_element(currency_history.begin(), currency_history.begin() + (currency_history.size() - 1) / 2, currency_history.end());
				median = (currency_history[(currency_history.size() - 1) / 2] + \
					currency_history[currency_history.size() / 2]) / 2.0;
			}
			//std::cout << "MED " << median << "\n";
			sql = "UPDATE currencies SET \'AVERAGE\' = " + std::to_string(avg) + ", \'MEDIAN\' = " + std::to_string(median) + " WHERE CHARCODE = \'" + char_code + "\'";
			request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
			if (request != SQLITE_OK) {
				std::cerr << "CRINGE: " << err << "\n";
				sqlite3_free(err);
			}

			if (!holder->contains(char_code)) {
				holder->add_value(char_code, Currency_info(currency_names[char_code], Currency_type::CRYPTO, value, avg, median));
			}
		}
	}
	std::cout << "Crypto parsing done" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() /1000000.0  << "\n";
}

std::atomic<bool> keep_going = true;
struct User_input {
	Currency_type type = Currency_type::ALL;
	std::set<std::string> specific_values{};
	std::pair<bool, std::string> json_enabled{false, "report"} ;
};
User_input get_input() {
		std::string input;
		//std::cin >> input;
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
		//promise_input.set_value(options);
}

void create_json(std::string name = "report") {
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
	std::ofstream output(name);
	output << result_json;
}

const std::chrono::system_clock::duration OFFSET = std::chrono::seconds(10);
int main() {
	system("chcp 65001 > nul");

	begin = std::chrono::steady_clock::now();
	open_table();
	 //char* err;
	 //std::string sql = "ALTER TABLE currencies DROP COLUMN \"Sat Mar 19 22:30:20 2022\"";
	 //sqlite3_exec(db, sql.c_str(), callback, nullptr, &err);
	 //exit(0);
	curl_global_init(CURL_GLOBAL_ALL);
	std::map<const std::string, Currency_info> currency_map;
	Currency_holder* holder = Currency_holder::get_instance(currency_map);
	User_input launch_settings{};
	std::future<User_input> future_input = std::async(std::launch::async, get_input);
	std::future_status input_status = std::future_status::deferred;
	try {
		while (keep_going) {
			auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			ctime_s(time_buf, sizeof time_buf, &timenow);
			time_buf[24] = '\0';
			std::future<void> regular_currency_res = std::async(std::launch::async, regular_currency_parsing, holder);
			std::future<void> crypto_currency_res = std::async(std::launch::async, crypto_currency_parse, holder, std::ref(regular_currency_res));
			crypto_currency_res.get();
			//regular_currency_res.get();
			if (launch_settings.type != Currency_type::NONE) {
				std::cout << "==== Actual currency course for " << time_buf << " ====\n";
				fort::char_table table;
				table << fort::header << "Charcode" << "Value" << "Average" << "Median" << "Name" << fort::endr;
				if (launch_settings.specific_values.size()) {
					for (auto const& iter : launch_settings.specific_values) {
						std::cout << iter << "\n";
						//table << iter;
						if (holder->contains(iter)) {
							Currency_info found = holder->value(iter);
							if (launch_settings.type == Currency_type::ALL || launch_settings.type == found.type) {
								table << iter << found.value << found.avg << found.median << found.name << fort::endr;
							}
						}
						else {
							table << iter << "No such currency" << "N/D" << "N/D" << "N/D" << fort::endr;
						}
					}
				}
				else {
					currency_map = holder->value();
					for (auto const& iter : currency_map) {
						if (launch_settings.type == Currency_type::ALL || launch_settings.type == iter.second.type) {
							table << iter.first << iter.second.value << iter.second.avg << iter.second.median << iter.second.name << fort::endr;
						}
					}
				}
				std::cout << table.to_string() << std::endl;
			}
			else {
				std::cout << "=== Currency course for " << time_buf << "is ready ===\n";
 			}
			if (launch_settings.json_enabled.first) {
				std::cout << "Preparing json report...\n";
				create_json(launch_settings.json_enabled.second);
				std::cout << "Your json report is ready\n";
			}
			std::cout << "" << "\n";
			std::chrono::system_clock::time_point offset_passed = std::chrono::system_clock::now() + std::chrono::seconds(60);
			auto launch_time = std::chrono::system_clock::to_time_t(offset_passed);
			ctime_s(time_buf, sizeof time_buf, &launch_time);
			std::cout << "Enter options for the next launch\nNote that it wil be automatically done approximately at " << time_buf;
			if (input_status == std::future_status::ready) {
				future_input = std::async(std::launch::async, get_input);
			}
			input_status = future_input.wait_until(offset_passed);
			if (input_status == std::future_status::ready) {
				launch_settings = future_input.get();
			}
			else {
				launch_settings = User_input{};
			}
			//Sleep(1000000);
			system("cls");
			std::cout << "ALIVE\n";
		}
	}
	catch (std::exception const& e) {
		std::cerr << e.what();
		holder->release_instance();
		curl_global_cleanup();
		sqlite3_close_v2(db);
	}
	std::cout << "Shutting down";
	holder->release_instance();
	curl_global_cleanup();
	sqlite3_close_v2(db);
	return 0;
}