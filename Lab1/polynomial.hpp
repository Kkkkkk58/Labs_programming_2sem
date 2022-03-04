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
#include <regex>

constexpr double EPS = 1e-9;
const double PI = std::acos(-1.L);
template <class T, class E = void>
class CPolynomial;


namespace Polynomial_helper {
    template<class T>
    struct is_complex : public std::false_type {};

    template<class T>
    struct is_complex<std::complex<T>> : public std::true_type {};

    template<class T>
    bool equals_zero(T const& value) {
        return (std::is_floating_point<T>::value && std::abs(value) <= EPS) ||
         (!std::is_floating_point<T>::value && value == T(0));
    }

    void fft(std::vector<std::complex<double>> &compl_coefs, bool const& invert) {
        size_t degree = compl_coefs.size();
        if (degree <= 1) {
            return;
        }
        std::vector<std::complex<double>> first_part(degree / 2);
        std::vector<std::complex<double>> second_part(degree / 2);
        for (int i = 0; i < degree; i += 2) {
            first_part[i / 2] = compl_coefs[i];
            second_part[i / 2] = compl_coefs[i + 1];
        }
        fft(first_part, invert);
        fft(second_part, invert);
        double argument = 2 * PI / degree * (invert ? -1 : 1);
        std::complex<double> root(1);
        std::complex<double> multiplier(std::cos(argument), std::sin(argument));
        for (int i = 0; i < degree / 2; ++i) {
            compl_coefs[i] = first_part[i] + root * second_part[i];
            compl_coefs[i + degree / 2] = first_part[i] - root * second_part[i];
            if (invert) {
                compl_coefs[i] /= 2;
                compl_coefs[i + degree / 2] /= 2;
            }
            root *= multiplier;
        }
    }

    template<class T>
    typename std::enable_if<std::is_integral<T>::value, std::map<size_t, T>>::type
    from_string(std::string const& str) {
        std::regex poly_template("\\+|\\-?(\\d+)?\\*?[a-zA-Z]?\\^?(\\d+)?");
        std::sregex_iterator rit(str.begin(),\
        str.end(), poly_template);
        std::sregex_iterator rend;
        std::map<size_t, T> coefficients;
        while (rit != rend) {
            if (std::string((*rit)[0]) != "+" && std::string((*rit)[0]).length()) {
                T value(0);
                int sign = 1;
                size_t degree = 0;
                if (std::string((*rit)[0])[0] == '-') {
                    sign = -1;
                }
                if (std::string((*rit)[1]).length()) {
                    value = sign * std::stoi(std::string((*rit)[1]));
                }
                else {
                    value = sign * T(1);
                }
                if (value == T(0)) {
                    continue;
                }
                if (std::string((*rit)[2]).length()) {
                    degree = stoi(std::string((*rit)[2]));
                }
                else {
                    if (std::string((*rit)[0]) != std::string((*rit)[1])) {
                        degree = 1;
                    }
                }
                if (coefficients.find(degree) != coefficients.end()) {
                    coefficients[degree] += value;
                }
                else {
                    coefficients[degree] = value;
                }
            }
            ++rit;
        }
        return coefficients;
    }

    template<class T>
    typename std::enable_if<std::is_floating_point<T>::value, std::map<size_t, T>>::type
    from_string(std::string const& str) {
        std::regex poly_template("\\+|\\-?(\\d*\\.)?(\\d+)?\\*?[a-zA-Z]?\\^?(\\d+)?");
        std::sregex_iterator rit(str.begin(),\
        str.end(), poly_template);
        std::sregex_iterator rend;
        std::map<size_t, T> coefficients;
        while (rit != rend) {
            if (std::string((*rit)[0]) != "+" && std::string((*rit)[0]).length()) {
                T value(0);
                int sign = 1;
                size_t degree = 0;
                if (std::string((*rit)[0])[0] == '-') {
                    sign = -1;
                }
                std::string num = std::string((*rit)[1]) + std::string((*rit)[2]);
                if (num.length()) {
                    value = sign * std::stod(num);
                }
                else {
                    value = sign * T(1);
                }
                if (value == T(0)) {
                    continue;
                }
                if (std::string((*rit)[3]).length()) {
                    degree = stoi(std::string((*rit)[3]));
                }
                else {
                    if (std::string((*rit)[0]) != num) {
                        degree = 1;
                    }
                }
                if (coefficients.find(degree) != coefficients.end()) {
                    coefficients[degree] += value;
                }
                else {
                    coefficients[degree] = value;
                }
            }
            ++rit;
        }
        return coefficients;
    }

    template<class T>
    typename std::enable_if<is_complex<T>::value, std::map<size_t, T>>::type
    from_string(std::string const& str) {
        std::regex poly_template("\\+|\\-?(\\((\\d*\\.)?(\\d+)?\\,?(\\d*\\.)?(\\d+)?\\))?\\*?[a-zA-Z]?\\^?(\\d+)?");
        std::sregex_iterator rit(str.begin(),\
        str.end(), poly_template);
        std::sregex_iterator rend;
        std::map<size_t, T> coefficients;
        while (rit != rend) {
            if (std::string((*rit)[0]) != "+" && std::string((*rit)[0]).length()) {
                T value(0);
                int sign = 1;
                size_t degree = 0;
                if (std::string((*rit)[0])[0] == '-') {
                    sign = -1;
                }
                if (std::string((*rit)[1]).length()) {
                    std::string re_str = std::string((*rit)[2]) + std::string((*rit)[3]);
                    double re = 0;
                    if (re_str.length()) {
                        re = std::stod(re_str);
                    }
                    std::string im_str = std::string((*rit)[4]) + std::string((*rit)[5]);
                    double im = 0;
                    if (im_str.length()) {
                        im = std::stod(im_str);
                    }
                    value = T(re, im);
                    value *= sign;
                }
                else {
                    value = T(1);
                    value *= sign;
                }
                if (value == T(0)) {
                    continue;
                }
                if (std::string((*rit)[6]).length()) {
                    degree = stoi(std::string((*rit)[6]));
                }
                else {
                    if (std::string((*rit)[0]) != std::string((*rit)[1])) {
                        degree = 1;
                    }
                }
                if (coefficients.find(degree) != coefficients.end()) {
                    coefficients[degree] += value;
                }
                else {
                    coefficients[degree] = value;
                }
            }
            ++rit;
        }
        return coefficients;
    }
}

template <class T>
class CPolynomial<T, typename std::enable_if<std::is_arithmetic<T>::value || \
Polynomial_helper::is_complex<T>::value>::type> {
public:
    explicit CPolynomial(std::vector<T> const& coefficients = std::vector<T>()) {
        for (size_t i = 0; i < coefficients.size(); ++i) {
            if (!Polynomial_helper::equals_zero(coefficients[i])) {
                coefficients_[i] = coefficients[i];
            }
        }
    }
    explicit CPolynomial(std::map<size_t, T> const& coefficients) : coefficients_(coefficients) {
        auto iter = coefficients_.begin();
        while (iter != coefficients_.end()) {
            if (Polynomial_helper::equals_zero(iter->second)) {
                iter = coefficients_.erase(iter);
            }
            else {
                ++iter;
            }
        }
    }
    ~CPolynomial() = default;
    explicit CPolynomial(std::initializer_list<T> const& coefficients) {
        size_t i = 0;
        for (auto iter = coefficients.begin(); iter != coefficients.end(); ++iter, ++i) {
            if (!Polynomial_helper::equals_zero(*iter)) {
                coefficients_[i] = *iter;
            }
        }
    }
    explicit CPolynomial(size_t size, T const& value = T(0)) {
        if (!Polynomial_helper::equals_zero(value)) {
            for (size_t i = 0; i <= size; ++i) {
                coefficients_[i] = value;
            }
        }
        else {
            coefficients_[size] = T(1);
        }
    }
    explicit CPolynomial(std::string str) : coefficients_(std::map<size_t, T>()) {
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        coefficients_ = Polynomial_helper::from_string<T>(str);
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
    bool operator==(CPolynomial const& other) const {
        return coefficients_ == other.coefficients_;
    }
    bool operator!=(CPolynomial const& other) const {
        return !(*this == other);
    }
    bool operator<(CPolynomial const& other) const {
        return coefficients_ < other.coefficients_;
    }
    bool operator>(CPolynomial const& other) const {
        return other < *this;
    }
    bool operator<=(CPolynomial const& other) const {
        return !(other < *this);
    }
    bool operator>=(CPolynomial const& other) const {
        return !(*this < other);
    }
    CPolynomial operator+() const {
        return *this;
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
    CPolynomial &operator+=(CPolynomial const& other) {
        for (size_t i = 0; i <= std::max(deg(), other.deg()); ++i) {
            auto this_finder = coefficients_.find(i);
            auto other_finder = other.coefficients_.find(i);
            if (this_finder != coefficients_.end() && other_finder != other.coefficients_.end()) {
                if (!Polynomial_helper::equals_zero(this_finder->second + other_finder->second)) {
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
    CPolynomial &operator*=(T const &value) {
        if (!Polynomial_helper::equals_zero(value)) {
            for (auto coef : coefficients_) {
                coefficients_[coef.first] = value * coef.second;
            }
        }
        else {
            std::map<size_t, T> empty_coefficients;
            coefficients_ = empty_coefficients;
        }
        return *this;
    }
    CPolynomial &operator*=(CPolynomial const& other) {
        std::vector<std::complex<double>> compl_first(deg() + 1);
        for (size_t i = 0; i <= deg(); ++i) {
            if (coefficients_.find(i) != coefficients_.end()) {
                compl_first[i] = std::complex<double>(coefficients_.at(i));
            }
        }
        std::vector<std::complex<double>> compl_second(other.deg() + 1);
        for (size_t i = 0; i <= other.deg(); ++i) {
            if (other.coefficients_.find(i) != other.coefficients_.end()) {
                compl_second[i] = std::complex<double>(other.coefficients_.at(i));
            }
        }
        size_t degree = 1;
        while (degree < std::max(compl_first.size(), compl_second.size())) {
            degree <<= 1;
        }
        degree <<= 1;
        compl_first.resize(degree);
        compl_second.resize(degree);
        Polynomial_helper::fft(compl_first, false);
        Polynomial_helper::fft(compl_second, false);
        for (size_t i = 0; i < degree; ++i) {
            compl_first[i] *= compl_second[i];
        }
        Polynomial_helper::fft(compl_first, true);
        std::vector<T> res(degree); 
        for (size_t i = 0; i < degree; ++i) {
            res[i] = compl_first[i].real();
        }
        *this = CPolynomial(res);
        return *this;
    }
    CPolynomial &operator/=(T const& value) {
        if (!Polynomial_helper::equals_zero(value)) {
            for (auto coef = coefficients_.begin(); coef != coefficients_.end(); ) {
                if (!Polynomial_helper::equals_zero(coef->second / value)) {
                    coef->second /= value;
                    std::cout << coefficients_[coef->first] << "\n";
                    ++coef;
                }
                else {
                    coef = coefficients_.erase(coef);
                }
            }
        }
        else {
            std::map<size_t, T> empty_coefficients;
            coefficients_ = empty_coefficients;
        }
        
        return *this;
    }
    CPolynomial &operator/=(CPolynomial const& other) {
        CPolynomial res;
        while (deg() >= other.deg()) {
            size_t res_deg = deg() - other.deg();
            CPolynomial sub = other * CPolynomial(res_deg);
            res += CPolynomial(res_deg);
            *this -= sub;
        }
        *this = res;
        return *this;
    }
    CPolynomial &operator%=(CPolynomial const& other) {
        while (deg() >= other.deg()) {
            size_t res_deg = deg() - other.deg();
            CPolynomial sub = other * CPolynomial(res_deg);
            *this -= sub;
        }
        return *this;
    }
    T operator[](size_t const& i) const {
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
                else if (degree == *p.coefficients_.begin()) {
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
        size_t i = 0;
        T coef;
        is >> coef;
        while (is.good()) {
            if (!Polynomial_helper::equals_zero(coef)) {
                p.coefficients_[i] = coef;
            }
            ++i;
            is >> coef;
        }
        return is;
    }
    T evaluation(T const& argument) const {
        if (!coefficients_.empty()) {
            T eval = coefficients_.rbegin()->second;
            for (int i = deg() - 1; i >= 0; --i) {
                eval *= argument;
                if (coefficients_.find(i) != coefficients_.end()) {
                    eval += coefficients_.at(i);
                }
            }
            return eval;
        }
        else {
            return 0;
        }
    }
    friend CPolynomial derivative(CPolynomial poly, size_t const& order) {
        for (size_t i = 0; i < order; ++i) {
            if (poly.deg() != -1) {
                auto curr = poly.coefficients_.erase(poly.coefficients_.find(0));
                while (curr != poly.coefficients_.end()) {
                    poly.coefficients_[curr->first - 1] = static_cast<T>(curr->first) * curr->second;
                    curr = poly.coefficients_.erase(curr);
                }
            }
            else {
                break;
            }
        }  
        return poly;
    }
private:
    std::map<size_t, T> coefficients_;
};


template<class T>
CPolynomial<T> operator+(CPolynomial<T> lhs, CPolynomial<T> const& rhs) {
    return lhs += rhs;
}

template<class T>
CPolynomial<T> operator-(CPolynomial<T> lhs, CPolynomial<T> const& rhs) {
    return lhs -= rhs;
}

template<class T>
CPolynomial<T> operator*(CPolynomial<T> lhs, double const& rhs) {
    return lhs *= rhs;
}

template<class T>
CPolynomial<T> operator*(double const& lhs, CPolynomial<T> rhs) {
    return rhs *= lhs;
}

template<class T>
CPolynomial<T> operator/(CPolynomial<T> lhs, double const& rhs) {
    return lhs /= rhs;
}

template<class T>
CPolynomial<T> operator*(CPolynomial<T> lhs, CPolynomial<T> const& rhs) {
    return lhs *= rhs;
}

template<class T>
CPolynomial<T> operator/(CPolynomial<T> lhs, CPolynomial<T> const& rhs) {
    return lhs /= rhs;
}

template<class T>
CPolynomial<T> operator%(CPolynomial<T> lhs, CPolynomial<T> const& rhs) {
    return lhs %= rhs;
}

#endif
