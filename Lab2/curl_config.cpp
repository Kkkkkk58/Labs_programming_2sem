#include "curl_config.hpp"

// ����������� ������ CURL_adapter
CURL_adapter::CURL_adapter() 
	: curl_(curl_easy_init()), error_buffer_(""), status_(CURLE_OK) {
	if (!curl_) {
		status_ = CURLE_FAILED_INIT;
	}
}

// ���������� ������ �������� ���������� ������ � ������� �����������
CURL_adapter::~CURL_adapter() {
	curl_easy_cleanup(curl_);
}

// ������ ��� ���� ���������������� ������� curl_easy_setopt
void CURL_adapter::setopt(std::string const& user_agent, std::string const& url, size_t(*func)(void* ptr, size_t size, size_t nmemb, std::string* data), std::string& response) {
	// ����������� error_buffer_ ��� ������ ��� ������ ������ ��� ������ � CURL
	curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, error_buffer_);
	// �������� User Agent
	curl_easy_setopt(curl_, CURLOPT_USERAGENT, user_agent.c_str());
	// �������� ������ ��� ��������� ����������
	curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
	// �������� ��������� �� �������, ��������������� ��� ������ ���������� �������
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, func);
	// ������ ����������� ������� � ����������
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
}

// ������ ��� ������ perform � ��������� �� ������������
void CURL_adapter::perform() {
	status_ = curl_easy_perform(curl_);
	if (status_ != CURLE_OK) {
		std::cerr << "Action failed:" << error_buffer_;
	}
}

// �������� �������� ���������� � bool - �������� �� ������������ ������� �������
CURL_adapter::operator bool() const {
	return status_ == CURLE_OK;
}

// �������� �������� ���������� � CURL* - ������� ��������� �� ������ ��
CURL_adapter::operator CURL* () const {
	return curl_;
}

