#include "curl_config.hpp"
#include "Parser_func.hpp"
#include "json/json.hpp"
#include "Currency.hpp"

char time_buf[26];

size_t write_response(void* ptr, size_t size, size_t nmemb, std::string* data) {
	if (data) {
		data->append(static_cast<char*>(ptr), size * nmemb);
		return size * nmemb;
	}
	else {
		return 0;
	}
}


void insert_new_value(Database& db, std::string const& char_code, Currency_info const& info) {
	std::string sql = "INSERT OR IGNORE INTO currencies(CHARCODE, NAME, TYPE) VALUES(\'" + char_code + "\',\'" + info.name + "\',\'" + (info.type == Currency_type::CRYPTO ? "crypto\')" : "regular\')");
	db.query(sql, callback);
	sql = "UPDATE currencies set \'" + std::string(time_buf) + "\' = " + std::to_string(info.value) + " where CHARCODE = \'" + char_code + "\'";
	db.query(sql, callback);
}

void get_history(Database &db, std::vector<double> &currency_history, std::string const& char_code) {
	std::string sql = "SELECT * FROM currencies WHERE CHARCODE = \'" + char_code + "\'";
	sqlite3_stmt* stmt;
	db.query(&stmt, sql);
	for (int i = 0; i < columns.size(); ++i) {
		currency_history[i] = sqlite3_column_double(stmt, i + 5);
	}
}

void count_avg_and_median(std::vector<double> &currency_history, double &avg, double &median) {
	avg = std::accumulate(currency_history.begin(), currency_history.end(), 0.0) / static_cast<double>(columns.size());
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
}

void update_db(Currency_holder &holder, Database& db, std::string const& char_code, Currency_info info) {
	insert_new_value(db, char_code, info);
	std::vector<double> currency_history(columns.size());
	get_history(db, currency_history, char_code);
	double avg, median;
	count_avg_and_median(currency_history, avg, median);
	info.avg = avg;
	info.median = median;
	holder.add_value(char_code, info);
	std::string sql = "UPDATE currencies SET \'AVERAGE\' = " + std::to_string(avg) + ", \'MEDIAN\' = " + std::to_string(median) + " WHERE CHARCODE = \'" + char_code + "\'";
	db.query(sql, callback);
}


void regular_currency_parsing(Currency_holder& holder, Database& db) {
	CURL_adapter curl;
	if (curl) {
		std::string response_data;
		curl.setopt(USER_AGENT, REGULAR_PARSE_URL, write_response, response_data);
		curl.perform();
		if (!curl) {
			return;
		}
		nlohmann::json parsed_currencies = nlohmann::json::parse(response_data);
		std::string sql = "ALTER TABLE currencies ADD \'" + std::string(time_buf) + "\' FLOAT NULL";
		db.query(sql, callback);
		columns.push_back(std::string(time_buf));
		for (auto const& item : parsed_currencies["Valute"].items()) {
			std::string char_code(item.key());
			std::string name(item.value()["Name"]);
			double value(item.value()["Value"]);
			update_db(holder, db, char_code, Currency_info{ name, Currency_type::REGULAR, value });
		}

	}
}


void crypto_currencies_list(std::map<const std::string, std::string>& full_names, std::atomic<bool>& status) {
	CURL_adapter curl;
	if (curl) {
		std::string response;
		curl.setopt(USER_AGENT, CRYPTO_LIST_URL, write_response, response);
		curl.perform();
		if (!curl) {
			return;
		}
		nlohmann::json parsed_names = nlohmann::json::parse(response);
		for (auto const& item : parsed_names["crypto"].items()) {
			full_names[item.key()] = (item.value()["name"]);
		}
	}
	status = true;
}


void crypto_currency_parse(Currency_holder& holder, Database& db, std::future<void>& regular) {
	std::map<const std::string, std::string> currency_names;
	std::atomic<bool> status = false;
	std::future<void> get_list = std::async(std::launch::async, crypto_currencies_list, std::ref(currency_names), std::ref(status));
	CURL_adapter curl;
	if (curl) {
		std::string response;
		curl.setopt(USER_AGENT, CRYPTO_PARSE_URL, write_response, response);
		curl.perform();
		if (!curl) {
			return;
		}
		nlohmann::json parsed_currencies = nlohmann::json::parse(response);
		regular.get();
		const double to_rubles = holder.get_curr_value("USD");
		for (auto const& item : parsed_currencies["rates"].items()) {
			std::string char_code(item.key());
			double value = static_cast<double>(item.value()) * to_rubles;
			if (!holder.contains(char_code) && !status) {
				get_list.get();
			}
			std::string name("");
			if (status) {
				name = currency_names[char_code];
			}
			update_db(holder, db, char_code, Currency_info{ name, Currency_type::CRYPTO, value });

		}
	}
}


