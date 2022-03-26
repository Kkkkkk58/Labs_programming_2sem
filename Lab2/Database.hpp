#ifndef DATABASE_HPP
#define DATABASE_HPP
#include "sqlite3/sqlite3.h"
#include <mutex>
#include <iostream>

int callback(void*, int, char**, char**);

// Класс для работы с базой данный SQLite3, реализованный по принципу Singleton
class Database {
public:
	static Database& get_instance(std::string const&);
	Database(Database const&) = delete;
	Database& operator=(Database const&) = delete;
	Database(Database&&) = default;
	Database& operator=(Database&&) = default;
	~Database();
	operator sqlite3* () const;
	void query(std::string const&, auto const);
	void query(sqlite3_stmt**, std::string const&);
private:
	static sqlite3* db_;
	Database() {}
	explicit Database(std::string const& db_name);
	static std::mutex mutex_;
};


extern std::vector<std::string> columns;
extern int rows_number;
Database& open_table(bool &);
#endif
