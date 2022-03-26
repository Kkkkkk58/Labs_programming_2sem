#include "curl_config.hpp"

// Конструктор класса CURL_adapter
CURL_adapter::CURL_adapter() 
	: curl_(curl_easy_init()), error_buffer_(""), status_(CURLE_OK) {
	if (!curl_) {
		status_ = CURLE_FAILED_INIT;
	}
}

// Деструктор класса вызывает завершение работы с текущим экземпляром
CURL_adapter::~CURL_adapter() {
	curl_easy_cleanup(curl_);
}

// Обёртка для пяти последовательных вызовов curl_easy_setopt
void CURL_adapter::setopt(std::string const& user_agent, std::string const& url, size_t(*func)(void* ptr, size_t size, size_t nmemb, std::string* data), std::string& response) {
	// Определение error_buffer_ как буфера для записи ошибок при работе с CURL
	curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, error_buffer_);
	// Передача User Agent
	curl_easy_setopt(curl_, CURLOPT_USERAGENT, user_agent.c_str());
	// Передача ссылки для получения информации
	curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
	// Передача указателя на функцию, предназначенную для записи результата запроса
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, func);
	// Запись результатов запроса в переменную
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
}

// Обёртка для метода perform с проверкой на корректность
void CURL_adapter::perform() {
	status_ = curl_easy_perform(curl_);
	if (status_ != CURLE_OK) {
		std::cerr << "Action failed:" << error_buffer_;
	}
}

// Оператор неявного приведения к bool - проверка на корректность статуса объекта
CURL_adapter::operator bool() const {
	return status_ == CURLE_OK;
}

// Оператор неявного приведения к CURL* - возврат указателя на объект БД
CURL_adapter::operator CURL* () const {
	return curl_;
}

