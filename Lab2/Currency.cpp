#include "Currency.hpp"


std::mutex Currency_holder::mutex_;
Currency_holder& Currency_holder::get_instance(std::map<const std::string, Currency_info> const& currency_map) {
	std::lock_guard<std::mutex> lock(mutex_);
	static Currency_holder holder(currency_map);
	return holder;
}
void Currency_holder::add_value(std::string const& char_code, Currency_info const& info) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (currencies_storage_.contains(char_code)) {
		std::string old_name(currencies_storage_[char_code].name);
		currencies_storage_[char_code] = info;
		if (info.name == "") {
			currencies_storage_[char_code].name = old_name;
		}
	}
	else {
		currencies_storage_.emplace(char_code, info);
	}
}
Currency_info Currency_holder::value(std::string const& char_code) const {
	if (contains(char_code)) {
		return currencies_storage_.at(char_code);
	}
	else {
		return Currency_info{};
	}
}

double Currency_holder::get_curr_value(std::string const& char_code) const {
	auto finder = currencies_storage_.find(char_code);
	if (finder == currencies_storage_.end()) {
		return 0;
	}
	else {
		return finder->second.value;
	}
}

