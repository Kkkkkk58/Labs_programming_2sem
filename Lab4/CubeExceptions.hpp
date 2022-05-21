#ifndef CUBE_EXCEPTIONS_HPP
#define CUBE_EXCEPTIONS_HPP
#include <iostream>
#include <string>
#include <exception>

// Базовый класс для исключений текущего проекта
class CubeException : public std::exception {
public:
	CubeException() {}
	CubeException(std::string const& err) : error_("ERR: " + err) {}
	const char* what() const override {
		return error_.c_str();
	}
private:
	std::string error_;
};

// Исключение "Неопределенный цвет"
class UnknownColour : public CubeException {
public:
	UnknownColour(std::string const& err) : CubeException(), error_("UnknownColour: " + err) {}
	const char* what() const override {
		return error_.c_str();
	}
private:
	std::string error_;
};

// Исключение "Неопределенный поворот"
class UnknownMove : public CubeException {
public:
	UnknownMove(std::string const& err) : CubeException(), error_("UnknownMove: " + err) {}
	const char* what() const override {
		return error_.c_str();
	}
private:
	std::string error_;
};

// Исключение "Некорректное состояние кубика"
class InvalidState : public CubeException {
public:
	InvalidState(std::string const& err) : CubeException(), error_("Invalid state: " + err) {}
	const char* what() const override {
		return error_.c_str();
	}

private:
	std::string error_;
};

// Исключение "Некорректный предоставленный файл"
class InvalidFile : public CubeException {
public:
	InvalidFile(std::string const& err) : CubeException(), error_("Invalid file: " + err) {}
	const char* what() const override {
		return error_.c_str();
	}

private:
	std::string error_;
};

#endif
