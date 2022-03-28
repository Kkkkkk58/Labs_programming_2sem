#include "Currency.hpp"

// Инициализация статического члена класса - мьютекса
std::mutex Currency_holder::mutex_;

// Конструктор класса Currency_holder от map, где ключ - код валюты, значение - информация о валюте
Currency_holder::Currency_holder(std::map<const std::string, Currency_info> const& map)
	: currencies_storage_(map) {}

// Функция, создающая экземпляр объекта типа Singleton класса Currency_holder
Currency_holder& Currency_holder::get_instance(std::map<const std::string, Currency_info> const& currency_map) {
	std::lock_guard<std::mutex> lock(mutex_);
	static Currency_holder holder(currency_map);
	return holder;
}

// Функция, возвращающая map с валютами и информацией о них, хранящуюся в объекте Currency_holder
std::map<const std::string, Currency_info> const& Currency_holder::value() const {
	return currencies_storage_;
}

// Функция, возвращающая информацию о заданной валюте
Currency_info Currency_holder::value(std::string const& char_code) const {
	try {
		return currencies_storage_.at(char_code);
	}
	catch (std::exception const& e) {
		throw std::logic_error("No currency with " + char_code + " char code is found");
	}
}

// Функция для добавления или информации о валюте
void Currency_holder::add_value(std::string const& char_code, Currency_info const& info) {
	std::lock_guard<std::mutex> lock(mutex_);
	// Если валюта уже имеется в списке
	if (contains(char_code)) {
		std::string old_name(currencies_storage_[char_code].name);
		currencies_storage_[char_code] = info;
		// Если в info не задано имя, оставляем старое
		if (info.name == "") {
			currencies_storage_[char_code].name = old_name;
		}
	}
	else {
		currencies_storage_.emplace(char_code, info);
	}
}

// Функция для возврата текущей стоимости определённой валюты
double Currency_holder::get_curr_value(std::string const& char_code) const {
	auto finder = currencies_storage_.find(char_code);
	if (finder == currencies_storage_.end()) {
		return 0;
	}
	else {
		return finder->second.value;
	}
}

// Функция для возврата количества валют в хранилище
size_t Currency_holder::size() const {
	return currencies_storage_.size();
}

// Функция для проверки на содержание определённой валюты в хранилище
bool Currency_holder::contains(std::string const& char_code) const {
	return currencies_storage_.contains(char_code);
}

