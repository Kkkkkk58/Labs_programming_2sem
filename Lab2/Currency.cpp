#include "Currency.hpp"

// ������������� ������������ ����� ������ - ��������
std::mutex Currency_holder::mutex_;

// ����������� ������ Currency_holder �� map, ��� ���� - ��� ������, �������� - ���������� � ������
Currency_holder::Currency_holder(std::map<const std::string, Currency_info> const& map)
	: currencies_storage_(map) {}

// �������, ��������� ��������� ������� ���� Singleton ������ Currency_holder
Currency_holder& Currency_holder::get_instance(std::map<const std::string, Currency_info> const& currency_map) {
	std::lock_guard<std::mutex> lock(mutex_);
	static Currency_holder holder(currency_map);
	return holder;
}

// �������, ������������ map � �������� � ����������� � ���, ���������� � ������� Currency_holder
std::map<const std::string, Currency_info> const& Currency_holder::value() const {
	return currencies_storage_;
}

// �������, ������������ ���������� � �������� ������
Currency_info Currency_holder::value(std::string const& char_code) const {
	try {
		return currencies_storage_.at(char_code);
	}
	catch (std::exception const& e) {
		throw std::logic_error("No currency with " + char_code + " char code is found");
	}
}

// ������� ��� ���������� ��� ���������� � ������
void Currency_holder::add_value(std::string const& char_code, Currency_info const& info) {
	std::lock_guard<std::mutex> lock(mutex_);
	// ���� ������ ��� ������� � ������
	if (contains(char_code)) {
		std::string old_name(currencies_storage_[char_code].name);
		currencies_storage_[char_code] = info;
		// ���� � info �� ������ ���, ��������� ������
		if (info.name == "") {
			currencies_storage_[char_code].name = old_name;
		}
	}
	else {
		currencies_storage_.emplace(char_code, info);
	}
}

// ������� ��� �������� ������� ��������� ����������� ������
double Currency_holder::get_curr_value(std::string const& char_code) const {
	auto finder = currencies_storage_.find(char_code);
	if (finder == currencies_storage_.end()) {
		return 0;
	}
	else {
		return finder->second.value;
	}
}

// ������� ��� �������� ���������� ����� � ���������
size_t Currency_holder::size() const {
	return currencies_storage_.size();
}

// ������� ��� �������� �� ���������� ����������� ������ � ���������
bool Currency_holder::contains(std::string const& char_code) const {
	return currencies_storage_.contains(char_code);
}

