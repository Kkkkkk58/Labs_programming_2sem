#define CURL_STATICLIB
#include <iostream>
#include <map>
#include <thread>
#include <ctime>
#include <future>
#include <chrono>
#include "curl_config.hpp"
#include "json/json.hpp"
#include "Database.hpp"
#include "Currency.hpp"
#include "Parser_func.hpp"
#include "User_interaction.hpp"
#include "fort/fort.hpp"


// Тело основной исполняемой программы
int main() {
	// Установка кодировки для корректной работы с кириллицей
	system("chcp 65001 > nul");
	// Открытие базы данных
	bool db_is_ok = true;
	Database &db = open_table(db_is_ok);
	// Если соединение с базой данных не установлено - заканчиваем работу
	if (!db_is_ok) {
		return EXIT_FAILURE;
	}
	std::map<const std::string, Currency_info> currency_map;
	// Получение объекта-одиночки хранилища валют
	Currency_holder& holder = Currency_holder::get_instance(currency_map);
	User_input launch_settings;
	// Создание фонового потока, ожидающего пользовательский ввод
	std::future<User_input> future_input = std::async(std::launch::async, get_input);
	// Задание изначального статуса для потока
	std::future_status input_status = std::future_status::deferred;
	// Инициализация глобального curl
	curl_global_init(CURL_GLOBAL_ALL);
	// Отлавливаем исключения
	try {
		// Пока пользователь не захотел прекратить работу
		while (keep_going) {
			// Получение текущего времени и запись в буфер
			auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			ctime_s(time_buf, sizeof time_buf, &timenow);
			time_buf[24] = '\0';
			// Запуск потока, получающего информацию об обычных валютах
			std::future<void> regular_currency_res = std::async(std::launch::async, regular_currency_parse, std::ref(holder), std::ref(db));
			// Запуск потока, получающего информацию о криптовалютах
			std::future<void> crypto_currency_res = std::async(std::launch::async, crypto_currency_parse, std::ref(holder), std::ref(db), std::ref(regular_currency_res));
			crypto_currency_res.get();
			// Очистка консоли, чтобы старая таблица оставалась на экране до появления новой
			system("cls");
			// Если нужно выводить таблицу
			if (launch_settings.type != Currency_type::NONE) {
				std::cout << "==== Actual currency course for " << time_buf << " ====\n";
				fort::char_table table;
				table << fort::header << "Charcode" << "Value" << "Average" << "Median" << "Name" << fort::endr;
				// Если нужны только конкретные валюты
				if (launch_settings.specific_values.size()) {
					for (auto const& iter : launch_settings.specific_values) {
						// Если такая валюта присутсвует
						if (holder.contains(iter)) {
							Currency_info found = holder.value(iter);
							// Добавляем в таблицу только если валюта не относится к незапрошенному типу
							if (launch_settings.type == Currency_type::ALL || launch_settings.type == found.type) {
								table << iter << found.value << found.avg << found.median << found.name << fort::endr;
							}
						}
						else {
							table << iter << "No such currency" << "N/D" << "N/D" << "N/D" << fort::endr;
						}
					}
				}
				// Иначе печатаем информацию о всех валютах с заданным типом
				else {
					currency_map = holder.value();
					for (auto const& iter : currency_map) {
						if (launch_settings.type == Currency_type::ALL || launch_settings.type == iter.second.type) {
							table << iter.first << iter.second.value << iter.second.avg << iter.second.median << iter.second.name << fort::endr;
						}
					}
				}
				// Печатаем таблицу с flush потока
				std::cout << table.to_string() << std::endl;
			}
			// Если таблица не нужна - просто информируем о готовности отчёта
			else {
				std::cout << "=== Currency course for " << time_buf << " is ready ===\n";
 			}
			// Если нужен отчёт в виде файла json
			if (launch_settings.json_enabled.first) {
				std::cout << "Preparing json report...\n";
				create_json(db, launch_settings.json_enabled.second);
				std::cout << "Your json report is ready\n";
			}
			std::cout << "\n";
			// Определение следующего времени запуска парсинга
			std::chrono::system_clock::time_point offset_passed = std::chrono::system_clock::now() + OFFSET;
			auto launch_time = std::chrono::system_clock::to_time_t(offset_passed);
			ctime_s(time_buf, sizeof time_buf, &launch_time);
			std::cout << "Enter options for the next launch\nNote that it wil be automatically done approximately at " << time_buf;
			// Работа с фоновым потоком пользовательского ввода
			if (input_status == std::future_status::ready) {
				future_input = std::async(std::launch::async, get_input);
			}
			input_status = future_input.wait_until(offset_passed);
			if (input_status == std::future_status::ready) {
				launch_settings = future_input.get();
			}
			else {
				launch_settings = User_input{};
			}
		}
	}
	catch (std::exception const& e) {
		std::cerr << e.what();
	}
	// Завершение работы с глобальным curl
	curl_global_cleanup();
	return EXIT_SUCCESS;
}