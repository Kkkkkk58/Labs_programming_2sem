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
std::set<std::string> columns;
int rows_number;

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
//static int vecd_callback(void* NotUsed, int argc, char** argv, char** azColName) {
//	int i;
//	std::vector<double>* record = static_cast<std::vector<double>*>(NotUsed);
//	for (i = 0; i < argc; i++) {
//		//std::cout << NotUsed << "\n";
//		record->push_back(std::stod(argv[i]));
//		//printf("%s = WHAT %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//	}
//	printf("\n");
//	return 0;
//}
//static int int_callback(void* NotUsed, int argc, char** argv, char** azColName) {
//	int i;
//	int* record = static_cast<int*>(NotUsed);
//	for (i = 0; i < argc; i++) {
//		//std::cout << NotUsed << "\n";
//		*record = std::stoi(argv[i]);
//		//printf("%s = WHAT %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//	}
//	printf("\n");
//	return 0;
//}
//static int string_callback(void* NotUsed, int argc, char** argv, char** azColName) {
//	int i;
//	std::string* record = static_cast<std::string*>(NotUsed);
//	for (i = 0; i < argc; i++) {
//		//std::cout << NotUsed << "\n";
//		*record = argv[i];
//		//printf("%s = WHAT %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//	}
//	printf("\n");
//	return 0;
//}
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
		columns.insert((*rit)[1]);
		++rit;
	}
	std::cout << "Table opened" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() << "\n";
}

const std::string USER_AGENT("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36");

struct Currency_info {
	Currency_info(std::string const& name_ = "", double value_ = 0, double avg_ = 0, double median_ = 0)
		: name(name_), value(value_), avg(avg_), median(median_) {}
	Currency_info(Currency_info const& other)
		: name(other.name), value(other.value), avg(other.avg), median(other.median) {}
	Currency_info &operator=(Currency_info const& other) {
		if (this != &other) {
			name = other.name;
			value = other.value;
			avg = other.avg;
			median = other.median;
		}
		return *this;
	}
	std::string name;
	double value, avg, median;
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
//auto get_from_log() {
//	std::ifstream log("log");
//	if (!log.is_open() || log.peek() == EOF) {
//		return std::map<const std::string, Currency_info>();
//	}
//	size_t currencies_count;
//	log >> currencies_count;
//	std::map<const std::string, Currency_info> currency_map;
//	for (size_t i = 0; i < currencies_count; ++i) {
//		std::string char_code;
//		size_t internal_size;
//		log >> char_code >> internal_size;
//		//std::cout << char_code << " "  << internal_size;
//		std::vector<double> currency_history(internal_size);
//		for (size_t j = 0; j < internal_size; ++j) {
//			log >> currency_history[j];
//			//std::cout << currency_history[j] << " ";
//		}
//		std::string currency_name;
//		log >> currency_name;
//		std::replace(currency_name.begin(), currency_name.end(), '_', ' ');
//		//std::cout << "NAME IS " << currency_name << "\n";
//		currency_map.emplace(char_code, Currency_info(currency_name, currency_history));
//	}
//	return currency_map;
//}

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
		columns.insert(std::string(time_buf));
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
			holder->add_value(char_code, Currency_info(name, value, avg, median));
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
		curl_easy_setopt(curl, CURLOPT_URL, "http://api.coinlayer.com/api/list?access_key=1e8afef2f2122543e3db2bc12172e13c");
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
		curl_easy_setopt(curl, CURLOPT_URL, "http://api.coinlayer.com/api/live?access_key=1e8afef2f2122543e3db2bc12172e13c");
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
			//if (holder->contains(char_code)) {
			//	holder->add_value(char_code, value);
			//}
			//else {
			//	if (!status) {
			//		get_list.get();
			//	}
			//	if (currency_names.contains(char_code)) {
			//		std::string currency_name(currency_names[char_code]);
			//		holder->add_value(char_code, Currency_info(currency_name, { value }));
			//	}
			//}
			char* err;
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
			if (!holder->contains(char_code) && !status) {
				get_list.get();
			}
			holder->add_value(char_code, Currency_info(currency_names[char_code], value, avg, median));
		}
	}
	std::cout << "Crypto parsing done" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() /1000000.0  << "\n";
}


std::string get_input() {
	std::string input;
	std::cin >> input;
	return input;
}

int main() {
	system("chcp 65001 > nul");
	begin = std::chrono::steady_clock::now();
	open_table();
	//std::map<const std::string, Currency_info> currency_map = get_from_log();
	//std::exit(EXIT_SUCCESS);
	curl_global_init(CURL_GLOBAL_ALL);
	std::map<const std::string, Currency_info> currency_map;
	Currency_holder* holder = Currency_holder::get_instance(currency_map);
	try {
		while (true) {
			auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			ctime_s(time_buf, sizeof time_buf, &timenow);
			time_buf[24] = '\0';
			std::future<void> regular_currency_res = std::async(std::launch::async, regular_currency_parsing, holder);
			std::future<void> crypto_currency_res = std::async(std::launch::async, crypto_currency_parse, holder, std::ref(regular_currency_res));
			crypto_currency_res.get();
			//regular_currency_res.get();
			//std::ofstream log("log");
			std::cout << "==== Actual currency course for " << time_buf << " ====\n";
			//log << holder->size() << std::endl;
			currency_map = holder->value();
			for (auto const& iter : currency_map) {
				std::cout << iter.first << " : ";
				//log << iter.first << ' ' << iter.second.values.size() << ' ';
				//for (auto const& value : iter.second.values) {
				//	log << value << ' ';
				//}
				std::cout << iter.second.value << " ";
				std::cout << iter.second.name << " " << iter.second.avg << " " << iter.second.median << "\n";
				//std::string name_buf(iter.second.name);
				//std::replace(name_buf.begin(), name_buf.end(), ' ', '_');
				//log << name_buf << std::endl;
				//char* err;
				//std::string sql = "INSERT OR IGNORE INTO currencies(CHARCODE, NAME, TYPE) VALUES(\'" + std::string(iter.first) + "\',\'" + name_buf + "\',\'real\')";
				////std::string sql = "INSERT INTO currencies(CHARCODE,NAME,TYPE) SELE"
				//int request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
				//if (request != SQLITE_OK) {
				//	std::cerr << "CRINGE: " << err << "\n";
				//	sqlite3_free(err);
				//}
				//sql = "ALTER TABLE currencies ADD \'" + std::string(time_buf) + "\' FLOAT NULL";
				//request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
				//if (request != SQLITE_OK) {
				//	std::cerr << "CRINGE: " << err << "\n";
				//	sqlite3_free(err);
				//}
				////sql = "INSERT INTO currencies SELECT \'" + std::string(time_buf) + "\' where CHARCODE = \'" + std::string(iter.first) + "\' VALUES (" + std::to_string(holder->get_curr_value(iter.first)) + ")";
				//sql = "UPDATE currencies set \'" + std::string(time_buf) + "\' = " + std::to_string(holder->get_curr_value(iter.first)) + " where CHARCODE = \'" + std::string(iter.first) + "\'";
				//request = sqlite3_exec(db, sql.c_str(), callback, 0, &err);
				//if (request != SQLITE_OK) {
				//	std::cerr << "CRINGE: " << err << "\n";
				//	sqlite3_free(err);
				//}
			}
			//std::cout << "Actual report is ready! Show it?" << "\n";
			//std::future<std::string> future_string = std::async(std::launch::async, get_input);
			//std::chrono::system_clock::time_point five_seconds_passed = std::chrono::system_clock::now() + std::chrono::seconds(5);
			//std::future_status status = future_string.wait_until(five_seconds_passed);
			//if (status == std::future_status::ready) {
			//	auto result = future_string.get();
			//	std::cout << "Fine";
			//}
			//else {
			//	std::cout << "FUCK";
			//}
			Sleep(1300000);
			std::cout << "ALIVE";
		}
	}
	catch (std::exception const& e) {
		std::cerr << e.what();
		holder->release_instance();
		curl_global_cleanup();
		sqlite3_close_v2(db);
	}
	holder->release_instance();
	curl_global_cleanup();
	sqlite3_close_v2(db);
	return 0;
}