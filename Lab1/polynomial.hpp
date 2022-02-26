#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <complex>
#include <type_traits>
#include "Complex.hpp"

template <class T, class E = void>
class CPolynomial;

template <class T>
class CPolynomial<T, typename std::enable_if<std::is_arithmetic<T>::value || \
std::is_same<T, Complex>::value>::type> {
public:
    CPolynomial(std::vector<T> const& coefficients = std::vector<T>()) {
        for (size_t i = 0; i < coefficients.size(); ++i) {
            if (coefficients[i] != T(0)) {
                coefficients_[i] = coefficients[i];
            }
        }
    }
    CPolynomial(std::map<size_t, T> const& coefficients) : coefficients_(coefficients) {}
    ~CPolynomial() = default;
    CPolynomial(std::string const& str_representation) {
        //tbd
    }
    CPolynomial(std::initializer_list<T> const& coefficients) {
        size_t i = 0;
        for (auto iter = coefficients.begin(); iter != coefficients.end(); ++iter, ++i) {
            if (*iter != T(0)) {
                coefficients_[i] = *iter;
            }
        }
    }
    CPolynomial(size_t size, T value) {
        if (value) {
            for (size_t i = 0; i < size; ++i) {
                coefficients_[i] = value;
            }
        }
    }
    CPolynomial(CPolynomial const& other) : coefficients_(other.coefficients_) {}
    void swap(CPolynomial &other) {
        std::swap(coefficients_, other.coefficients_);
    } 
    CPolynomial(CPolynomial && other) { swap(other); }
    CPolynomial &operator=(CPolynomial const& other) {
        if (this != &other) {
            coefficients_ = other.coefficients_;
        }
        return *this;
    }
    CPolynomial &operator=(CPolynomial && other) {
        swap(other);
        return *this;
    }
    int deg() const {
        if (!coefficients_.empty()) {
            return coefficients_.rbegin()->first;
        }
        else {
            return -1;
        }
    }
    bool operator==(CPolynomial const& other) {
        return coefficients_ == other.coefficients_;
    }
    bool operator!=(CPolynomial const& other) {
        return !(*this == other);
    }
    bool operator<(CPolynomial const& other) {
        if (deg() == other.deg()) {
            // for (size_t i = deg(); i >= 0; --i) {
            //     if 
            // }
        }
        return deg() < other.deg();
    }
    //bool operator> , >=, <=
    CPolynomial operator+() const {
        return CPolynomial(*this);
    }
    CPolynomial operator-() const {
        std::map<size_t, T> new_coefficients;
        for (size_t i = 0; i <= deg(); ++i) {
            auto finder = coefficients_.find(i);
            if (finder != coefficients_.end()) {
                new_coefficients[i] = -(finder->second);
            }
        }
        return CPolynomial(new_coefficients);
    }
    CPolynomial operator+(CPolynomial const& other) const {
        std::map<size_t, T> new_coefficients;
        for (size_t i = 0; i <= std::max(deg(), other.deg()); ++i) {
            auto this_finder = coefficients_.find(i);
            auto other_finder = other.coefficients_.find(i);
            if (this_finder != coefficients_.end() || other_finder != other.coefficients_.end()) {
                if (this_finder != coefficients_.end() && other_finder != other.coefficients_.end() \
                && this_finder->second != -(other_finder->second)) {
                    new_coefficients[i] = this_finder->second + other_finder->second;
                }
                else if (this_finder != coefficients_.end() && other_finder == other.coefficients_.end()) {
                    new_coefficients[i] = this_finder->second;
                }
                else if (this_finder == coefficients_.end() && other_finder != other.coefficients_.end()) {
                    new_coefficients[i] = other_finder->second;
                }
            }
        }
        return CPolynomial(new_coefficients);
    }
    CPolynomial operator-(CPolynomial const& other) {
        return CPolynomial(*this + (-other));
    }
    CPolynomial &operator+=(CPolynomial const& other) {
        for (size_t i = 0; i <= std::max(deg(), other.deg()); ++i) {
            auto this_finder = coefficients_.find(i);
            auto other_finder = other.coefficients_.find(i);
            if (this_finder != coefficients_.end() && other_finder != other.coefficients_.end()) {
                if (this_finder->second != -(other_finder->second)) {
                    coefficients_[i] = this_finder->second + other_finder->second;
                }
                else {
                    coefficients_.erase(i);
                }
            }
            else if (this_finder == coefficients_.end() && other_finder != other.coefficients_.end())
            {
                coefficients_[i] = other_finder->second;
            }
        }
        return *this;
    }
    CPolynomial &operator-=(CPolynomial const& other) {
        *this += (-other);
        return *this;
    }
    friend CPolynomial operator*(double const& value, CPolynomial const& polynomial) {
        if (value != 0) {
            std::map<size_t, T> new_coefficients;
            for (auto coef : polynomial.coefficients_) {
                new_coefficients[coef.first] = value * coef.second;
            }
            return CPolynomial(new_coefficients);
        }
        return CPolynomial();
    }
    friend CPolynomial operator*(CPolynomial const& polynomial, double const& value) {
        return value * polynomial;
    }
    CPolynomial operator*(CPolynomial const& other) {
        //
    }
    CPolynomial &operator*=(double value) {
        *this = *this * value;
        return *this;
    }
    CPolynomial &operator*=(CPolynomial const& other) {
        //
    }
    CPolynomial operator/(double value) {
        return *this * (1 / value);
    }
    CPolynomial operator/(CPolynomial const& other) {
        //
    }
    CPolynomial operator%(CPolynomial const& other) {
        //
    }
    CPolynomial &operator/=(double value) {
        *this = *this / value;
        return *this;
    }
    CPolynomial &operator/=(CPolynomial const& other) {
        //
    }
    T operator[](size_t i) const {
        auto coef = coefficients_.find(i);
        if (coef != coefficients_.end()) {
            return coef->second;
        }
        return T(0);
    }
    friend std::ostream& operator<<(std::ostream &os, CPolynomial const &p) { 
        if (p.coefficients_.size() == 0) {
            os << T(0);
        }
        else {
            for (auto degree : p.coefficients_) {
                if (degree.first == 0) {
                    os << degree.second;
                }
                else if (degree == *(p.coefficients_.begin())) {
                    if (degree.first == 1) {
                        os << degree.second << "x";
                    }
                    else {
                        os << degree.second << "x^" << degree.first;
                    }
                }
                else {
                    if (degree.first == 1) {
                        os << " + " << degree.second << "x";
                    }
                    else {
                        os << " + " << degree.second << "x^" << degree.first;
                    }
                }
            }
        }
        return os; 
    }
    friend std::istream& operator>>(std::istream &is, CPolynomial &p) {
        //
        return is;
    }
private:
    std::map<size_t, T> coefficients_;
};

// template <class T>
// std::ostream& operator<<(std::ostream &os, CPolynomial<T> const &p);

#endif