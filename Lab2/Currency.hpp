#ifndef CURRENCY_HPP
#define CURRENCY_HPP
#include <string>
#include <map>
#include <mutex>
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
	Currency_holder(Currency_holder const&) = delete;
	Currency_holder& operator=(Currency_holder const&) = delete;
	Currency_holder(Currency_holder&&) = default;
	Currency_holder& operator=(Currency_holder&&) = default;
	static Currency_holder& get_instance(std::map<const std::string, Currency_info> const&);
	std::map<const std::string, Currency_info> const& value() const {
		return currencies_storage_;
	}
	Currency_info value(std::string const& char_code) const;
	void add_value(std::string const&, Currency_info const&);
	size_t size() const {
		return currencies_storage_.size();
	}
	bool contains(std::string const& char_code) const {
		return currencies_storage_.contains(char_code);
	}
	double get_curr_value(std::string const&) const;
private:
	std::map<const std::string, Currency_info> currencies_storage_;
	Currency_holder() {}
	Currency_holder(std::map<const std::string, Currency_info> const& map)
		: currencies_storage_(map) {}
	~Currency_holder() = default;
	static std::mutex mutex_;
};

#endif
