#include "curl_config.hpp"

CURL_adapter::CURL_adapter() 
	: curl_(curl_easy_init()), error_buffer_(""), status_(CURLE_OK) {
	if (!curl_) {
		status_ = CURLE_FAILED_INIT;
	}
}

void CURL_adapter::setopt(std::string const& user_agent, std::string const& url, size_t(*func)(void* ptr, size_t size, size_t nmemb, std::string* data), std::string& response) {
	curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, error_buffer_);
	curl_easy_setopt(curl_, CURLOPT_USERAGENT, user_agent.c_str());
	curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, func);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
}
void CURL_adapter::perform() {
	status_ = curl_easy_perform(curl_);
	if (status_ != CURLE_OK) {
		std::cerr << "Action failed:" << error_buffer_;
	}
}