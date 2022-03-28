#include "curl_config.hpp"
#include "Parser_func.hpp"
#include "json/json.hpp"
#include "Currency.hpp"

// Буфер для работы с текущим временем
char time_buf[26];

// Функция для записи данных, полученных при работе с CURL в строку
size_t write_response(void* ptr, size_t size, size_t nmemb, std::string* data) {
	if (data) {
		data->append(static_cast<char*>(ptr), size * nmemb);
		return size * nmemb;
	}
	else {
		return 0;
	}
}

// Функция для вставки нового значения в базу данных
void insert_new_value(Database& db, std::string const& char_code, Currency_info const& info) {
	std::string sql = "INSERT OR IGNORE INTO currencies(CHARCODE, NAME, TYPE) VALUES(\'" + char_code + "\',\'" + info.name + "\',\'" + (info.type == Currency_type::CRYPTO ? "crypto\')" : "regular\')");
	db.query(sql, callback);
	sql = "UPDATE currencies set \'" + std::string(time_buf) + "\' = " + std::to_string(info.value) + " where CHARCODE = \'" + char_code + "\'";
	db.query(sql, callback);
}

// Функция для получения вектора значений стоимости валюты за измеренный промежуток
void get_history(Database &db, std::vector<double> &currency_history, std::string const& char_code) {
	std::string sql = "SELECT * FROM currencies WHERE CHARCODE = \'" + char_code + "\'";
	sqlite3_stmt* stmt;
	db.query(&stmt, sql);
	for (int i = 0; i < columns.size(); ++i) {
		currency_history[i] = sqlite3_column_double(stmt, i + 5);
	}
}

// Функция для получения среднего и медианного значения вектора
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

// Функция для обновления состояния базы данных и хранилища валют
void update_state(Currency_holder &holder, Database& db, std::string const& char_code, Currency_info &info) {
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

// Функция для парсинга обычной валюты с сайта ЦБ РФ
void regular_currency_parse(Currency_holder& holder, Database& db) {
	CURL_adapter curl;
	if (curl) {
		// Выполнение запроса
		std::string response_data;
		curl.setopt(USER_AGENT, REGULAR_PARSE_URL, write_response, response_data);
		curl.perform();
		if (!curl) {
			return;
		}
		// Преобразование строковой информации, пришедшец с сайта в json
		nlohmann::json parsed_currencies = nlohmann::json::parse(response_data);
		// Добавление в таблицу колонки с текущим временем
		std::string sql = "ALTER TABLE currencies ADD \'" + std::string(time_buf) + "\' FLOAT NULL";
		db.query(sql, callback);
		columns.push_back(std::string(time_buf));
		// Разбор json и обновление значений
		for (auto const& item : parsed_currencies["Valute"].items()) {
			std::string char_code(item.key());
			std::string name(item.value()["Name"]);
			double value(item.value()["Value"]);
			Currency_info info{ name, Currency_type::REGULAR, value };
			update_state(holder, db, char_code, info);
		}

	}
}

// Функция для получения списка названий криптовалют
void crypto_currencies_list(std::map<const std::string, std::string>& full_names, std::atomic<bool>& status) {
	CURL_adapter curl;
	if (curl) {
		// Выполнение запроса
		std::string response;
		curl.setopt(USER_AGENT, CRYPTO_LIST_URL, write_response, response);
		curl.perform();
		if (!curl) {
			return;
		}
		// Парсинг json
		nlohmann::json parsed_names = nlohmann::json::parse(response);
		for (auto const& item : parsed_names["crypto"].items()) {
			full_names[item.key()] = (item.value()["name"]);
		}
	}
	status = true;
}

// Функция для парсинга криптовалюты
void crypto_currency_parse(Currency_holder& holder, Database& db, std::future<void>& regular) {
	std::map<const std::string, std::string> currency_names;
	std::atomic<bool> status = false;
	// Запуск потока, получающего список названий криптовалют
	std::future<void> get_list = std::async(std::launch::async, crypto_currencies_list, std::ref(currency_names), std::ref(status));
	CURL_adapter curl;
	if (curl) {
		// Выполнение запроса на получение стоимостей валют
		std::string response;
		curl.setopt(USER_AGENT, CRYPTO_PARSE_URL, write_response, response);
		curl.perform();
		if (!curl) {
			return;
		}
		// Создание json из полученной строки
		nlohmann::json parsed_currencies = nlohmann::json::parse(response);
		// Получение текущего значения стоимости USD для перевода криптовалют в рубли
		regular.get();
		const double to_rubles = holder.get_curr_value("USD");
		// Разбор json
		for (auto const& item : parsed_currencies["rates"].items()) {
			std::string char_code(item.key());
			double value = static_cast<double>(item.value()) * to_rubles;
			// Если мы не знаем имя валюты с таким кодом - ждём получения списка
			if (!holder.contains(char_code) && !status) {
				get_list.get();
			}
			// Название криптовалюты
			std::string name("");
			if (status) {
				name = currency_names[char_code];
			}
			// Обновление значений в базе дынных и хранилище валют
			Currency_info info{name, Currency_type::CRYPTO, value};
			update_state(holder, db, char_code, info);

		}
	}
}
