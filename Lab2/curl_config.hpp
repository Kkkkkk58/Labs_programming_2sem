#ifndef CONFIG_HPP
#define CONFIG_HPP
#define CURL_STATICLIB
#include "curl/curl.h"
#include <iostream>
#include <string>
#include <chrono>
const std::string USER_AGENT("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36");
const std::string REGULAR_PARSE_URL("https://www.cbr-xml-daily.ru/daily_json.js");
const std::string CRYPTO_LIST_URL("http://api.coinlayer.com/api/list?access_key=4ed624e0558531621863f52d78349089");
const std::string CRYPTO_PARSE_URL("http://api.coinlayer.com/api/live?access_key=4ed624e0558531621863f52d78349089");
const std::chrono::system_clock::duration OFFSET = std::chrono::seconds(10);

class CURL_adapter {
public:
	CURL_adapter();
	~CURL_adapter() {
		curl_easy_cleanup(curl_);
	}
	operator bool () const {
		return status_ == CURLE_OK;
	}
	void setopt(std::string const&, std::string const&, size_t (*)(void* ptr, size_t size, size_t nmemb, std::string* data), std::string&);
	void perform();
	operator CURL* () const {
		return curl_;
	}
private:
	CURL* curl_;
	char error_buffer_[CURL_ERROR_SIZE];
	CURLcode status_;
};
#endif
