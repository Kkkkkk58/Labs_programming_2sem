#ifndef CURRENCY_HPP
#define CURRENCY_HPP
#include <string>
#include <map>
#include <mutex>

// Класс перечисления, задающий типы валют
enum class Currency_type {
	NONE = -1,
	ALL,
	REGULAR,
	CRYPTO
};

// Структура с информацией о валюте
struct Currency_info {
	std::string name = "";
	Currency_type type = Currency_type::ALL;
	double value = 0, avg = 0, median = 0;
};

// Класс для хранения текущей информации о каждой валюте, реализованный по принципу Singleton
class Currency_holder {
public:
	Currency_holder(Currency_holder const&) = delete;
	Currency_holder& operator=(Currency_holder const&) = delete;
	Currency_holder(Currency_holder&&) = default;
	Currency_holder& operator=(Currency_holder&&) = default;
	static Currency_holder& get_instance(std::map<const std::string, Currency_info> const&);
	std::map<const std::string, Currency_info> const& value() const;
	Currency_info value(std::string const& char_code) const;
	void add_value(std::string const&, Currency_info const&);
	size_t size() const;
	bool contains(std::string const& char_code) const;
	double get_curr_value(std::string const&) const;
private:
	std::map<const std::string, Currency_info> currencies_storage_;
	Currency_holder() {}
	Currency_holder(std::map<const std::string, Currency_info> const& map);
	~Currency_holder() = default;
	static std::mutex mutex_;
};

#endif
