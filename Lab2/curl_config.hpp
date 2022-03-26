#ifndef CONFIG_HPP
#define CONFIG_HPP
#define CURL_STATICLIB
#include "curl/curl.h"
#include <iostream>
#include <string>
#include <chrono>

// ���������, ������������ User Agent ��� �������� ������
const std::string USER_AGENT("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36");
// ���������, �������� ������ �� ���� � API �� �� ��� �������� ������� ������
const std::string REGULAR_PARSE_URL("https://www.cbr-xml-daily.ru/daily_json.js");
// ���������, �������� ������ �� ���� � API, ����������� �������� ������ �����������
const std::string CRYPTO_LIST_URL("http://api.coinlayer.com/api/list?access_key=4ed624e0558531621863f52d78349089");
// ���������, �������� ������ �� ���� � API ��� �������� ������������
const std::string CRYPTO_PARSE_URL("http://api.coinlayer.com/api/live?access_key=4ed624e0558531621863f52d78349089");
// ���������, ������������ ������������� ��������� ������ � ������
const std::chrono::system_clock::duration OFFSET = std::chrono::seconds(60);

// �����-������ ��� ������� ���� CURL *
class CURL_adapter {
public:
	CURL_adapter();
	CURL_adapter(CURL_adapter const&) = delete;
	CURL_adapter& operator=(CURL_adapter const&) = delete;
	~CURL_adapter();
	using Func = size_t(*)(void* ptr, size_t size, size_t nmemb, std::string* data);
	void setopt(std::string const&, std::string const&, Func, std::string&);
	void perform();
	operator bool() const;
	operator CURL* () const;
private:
	CURL* curl_;
	char error_buffer_[CURL_ERROR_SIZE];
	CURLcode status_;
};

#endif
