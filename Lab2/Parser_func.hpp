#ifndef PARSER_FUNC_HPP
#define PARSER_FUNC_HPP
#include "curl/curl.h"
#include "Database.hpp"
#include "Currency.hpp"
#include <future>

static size_t write_response(void*, size_t, size_t, std::string*);
void regular_currency_parse(Currency_holder&, Database&);
void crypto_currencies_list(std::map<const std::string, std::string>& full_names, std::atomic<bool>& status);
void crypto_currency_parse(Currency_holder& holder, Database& db, std::future<void>& regular);

extern char time_buf[26];
#endif
