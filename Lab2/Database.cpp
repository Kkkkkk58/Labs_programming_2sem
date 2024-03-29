#include "Database.hpp"
#include <iostream>
#include <regex>

// Функция для обработки SQL-запросов, не требующих возврата значения
int callback(void* not_used, int argc, char** argv, char** az_col_name) {
	for (int i = 0; i < argc; i++) {
		std::cout << az_col_name[i] << " = " << (argv[i] ? argv[i] : "NULL");
	}
	std::cout << "\n";
	return SQLITE_OK;
}
// Вектор с названиями колонок базы данных
std::vector<std::string> columns;
// Количество строк в базе данных
int rows_number;

// Конструктор класса Database с указанным названием базы данных
Database::Database(std::string const& db_name) {
	int response = sqlite3_open(db_name.c_str(), &db_);
	// Если возникла ошибка при открытии
	if (response != SQLITE_OK) {
		throw std::logic_error("Unable to open the database");
	}
}

// Функция для получения объекта типа Singleton класса Database
Database& Database::get_instance(std::string const& db_name) {
	std::lock_guard<std::mutex> lock(mutex_);
	try {
		static Database db_wrapper(db_name);
		return db_wrapper;
	}
	// Если возникли ошибки при создании экземпляра класса
	catch (std::exception const& err) {
		throw std::logic_error(err.what() + std::string(" : ") + sqlite3_errmsg(db_));
	}
}

// Деструктор класса Database, закрывающий базу данных при разрушении объекта
Database::~Database() {
	std::lock_guard<std::mutex> lock(mutex_);
	int response = sqlite3_close_v2(db_);
	// Если разорвать соединение не удалось
	if (response != SQLITE_OK) {
		std::cerr << sqlite3_errmsg(db_);
	}
}

// Функция для обработки SQL-запросов без возвращаемых значений
void Database::query(std::string const& sql, auto const func) {
	char* err_msg;
	int response = sqlite3_exec(db_, sql.c_str(), func, nullptr, &err_msg);
	// Если выполнить запрос не удалось
	if (response != SQLITE_OK) {
		std::string err(err_msg);
		sqlite3_free(err_msg);
		throw std::logic_error(err);
	}
}

// Функция для обработки запросов типа sqlite3_stmt, возвращающих значения
void Database::query(sqlite3_stmt** stmt, std::string const& sql) {
	int response = sqlite3_prepare_v2(db_, sql.c_str(), -1, stmt, nullptr);
	// Если возникли проблемы при подготовке SQL-запроса
	if (response != SQLITE_OK) {
		throw std::logic_error("Unable to do the query");
	}
	sqlite3_step(*stmt);
}

// Функция неявного преобразования в sqlite3*
Database::operator sqlite3* () const {
	return db_; 
}

// Инициализация статических членов класса - указателя на sqlite3 и мьютекса
sqlite3* Database::db_;
std::mutex Database::mutex_;

// Функция для инициализации работы с таблицей с валютами
Database& open_table(bool &db_is_ok) {
	// Отлавливаем ошибки, которые могут возникнуть на различных шагах
	try {
		static Database& db = Database::get_instance("CurrenciesDB.db");
		// Создаём таблицу (если ещё не существует) внутри базы данных
		std::string sql("CREATE TABLE IF NOT EXISTS currencies (CHARCODE VARCHAR(10) PRIMARY KEY  NOT NULL, NAME NVARCHAR(100) NOT NULL, TYPE VARCHAR(10) NOT NULL, AVERAGE FLOAT NULL, MEDIAN FLOAT NULL);");
		db.query(sql, callback);
		sqlite3_stmt* stmt;
		// Получение колмчества строк в таблице
		sql = "SELECT COUNT(*) FROM currencies";
		db.query(&stmt, sql);
		rows_number = sqlite3_column_int(stmt, 0);
		sqlite3_reset(stmt);
		// Получение имён столбцов таблицы
		sql = "SELECT sql FROM sqlite_master WHERE tbl_name = \'currencies\' AND type = \'table\';";
		db.query(&stmt, sql);
		std::string col_names = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		sqlite3_finalize(stmt);
		std::regex temp("\'([a-zA-Z0-9: ]+)\' FLOAT NULL");
		std::sregex_iterator rit(col_names.begin(), col_names.end(), temp);
		std::sregex_iterator rend;
		while (rit != rend) {
			columns.push_back((*rit)[1]);
			++rit;
		}
		return db;
	}
	catch (std::exception const& err) {
		std::cerr << err.what();
		db_is_ok = false;
	}
}