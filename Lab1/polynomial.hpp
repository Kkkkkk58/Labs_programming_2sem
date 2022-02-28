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

namespace {
    template<class T>
    struct is_complex : public std::false_type {};

    template<class T>
    struct is_complex<std::complex<T>> : public std::true_type {};

    void multiply(std::vector<std::complex<double>> & first, std::vector<std::complex<double>> const& second) {
        for (size_t i = 0; i < first.size(); ++i) {
            first[i] *= second[i];
        }
    }

    void fft(std::vector<std::complex<double>> &vec, bool const& invert) {

        //EMAXX
        // size_t n = vec.size();
        // if (n <= 1) {
        //     return;
        // }
        // std::vector<std::complex<double>> a0(n / 2), a1(n / 2);
        // for (size_t i = 0, j = 0; i < n; i += 2, ++j) {
        //     a0[j] = vec[i];
        //     a1[j] = vec[i + 1];
        // }
        // fft(a0, invert);
        // fft(a1, invert);

        // double ang = 2 * std::acos(-1.L) / n * (invert ? -1 : 1);
        // std::complex<double> w(1), wn(std::cos(ang), std::sin(ang));
        // for (size_t i = 0; i < n / 2; ++i) {
        //     vec[i] = a0[i] + w * a1[i];
        //     vec[i + n / 2] = a0[i] - w * a1[i];
        //     if (invert) {
        //         vec[i] /= 2;
        //         vec[i + n / 2] /= 2;
        //     }
        //     w *= wn;
        // }

        // HABR
        // size_t n = vec.size();
        // if (n <= 1) {
        //     return;
        // }
        // std::vector<std::complex<double>> w(n);
        // for (size_t i = 0; i < n; ++i) {
        //     double alpha = 2 * std::acos(-1.L) * i / n;
        //     w[i] = std::complex<double>(std::cos(alpha), std::sin(alpha));
        // }
        // std::vector<std::complex<double>> a0(n / 2), a1(n / 2);
        // for (size_t i = 0; i < n / 2; ++i) {
        //     a0[i] = vec[2 * i];
        //     a1[i] = vec[2 * i + 1];
        // }
        // fft(a0, invert);
        // fft(a1, invert);
        // for (size_t i = 0; i < n; ++i) {
        //     vec[i] = a0[i % (n / 2)] + w[i] * a1[i % (n / 2)];
        //     if (invert) {
        //         vec[i] /= 2;
        //     }
        // }

    }
}

template <class T, class E = void>
class CPolynomial;

template <class T>
class CPolynomial<T, typename std::enable_if<std::is_arithmetic<T>::value || \
is_complex<T>::value>::type> {
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
    CPolynomial &operator*=(double value) {
        if (value != 0) {
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
        std::vector<std::complex<double>> vec_first(deg());
        for (size_t i = 0; i < deg(); ++i) {
            if (coefficients_.find(i) != coefficients_.end()) {
                vec_first[i] = coefficients_[i];
            }
        }
        std::vector<std::complex<double>> vec_second(other.deg());
        for (size_t i = 0; i < other.deg(); ++i) {
            if (other.coefficients_.find(i) != other.coefficients_.end()) {
                vec_second[i] = other.coefficients_.at(i);
            }
        }
        size_t n = 1;
        while (n < std::max(vec_first.size(), vec_second.size())) {
            n <<= 1;
        }
        n <<= 1;
        vec_first.resize(n);
        vec_second.resize(n);
        fft(vec_first, false);
        fft(vec_second, false);
        multiply(vec_first, vec_second);
        fft(vec_first, true);
        std::vector<T> res(n); 
        for (size_t i = 0; i < n; ++i) {
            res[i] = vec_first[i].real();
            //std::cout << res[i] << " ";
        }
        //std::cout << "\n";
        *this = CPolynomial(res);
        return *this;
    }
    CPolynomial &operator/=(double value) {
        *this *= (1 / value);
        return *this;
    }
    CPolynomial &operator/=(CPolynomial const& other) {
        //
    }
    CPolynomial &operator%=(CPolynomial const& other) {
        //
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
            if (coef != T(0)) {
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
                    poly.coefficients_[curr->first - 1] = curr->first * curr->second;
                    curr = poly.coefficients_.erase(poly.coefficients_.find(curr->first));
                }
            }
            else {
                break;
            }
        }  
        return poly;
    }
    // CPolynomial(std::string const& str_representation) : coefficients_(std::map<size_t, T>()) {
    //     // *this = from_string(str_representation);
    // }
    // CPolynomial from_string(std::string const& str_representation) const;
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

// template <class T>
// std::ostream& operator<<(std::ostream &os, CPolynomial<T> const &p);

#endif

//     CPolynomial(std::string const& str_representation) : coefficients_(std::map<size_t, T>()) {
//         std::string copy(str_representation);
//         copy.erase(std::remove(copy.begin(), copy.end(), ' '), copy.end());
//         std::regex poly_template("\\+|\\-?(\\d+)?\\*?[a-zA-Z]?\\^?(\\d+)?");
//         std::sregex_iterator rit(copy.begin(),\
//          copy.end(), poly_template);
//         std::sregex_iterator rend;
//         while (rit != rend) {
//             // std::string reg((*rit)[0]);
//             // if (reg.length() > 1 || reg != "+") {
//             //     size_t reg_pos = 0;
//             //     char sign = '+';
//             //     if (reg[0] == '-' || reg[0] == '+') {
//             //         ++reg_pos;
//             //         if (reg[0]) == '-' {
//             //             sign = '-';
//             //         }
//             //     }


//             // }
//             if (std::string((*rit)[0]) != "+") {
//                 T value;
//                 int sign = 1;
//                 size_t degree;
//                 if (std::string((*rit)[0])[0] == '-') {
//                     sign = -1;
//                 }
//                 if (std::string((*rit)[1]).length()) {
//                     if (std::is_integral<T>::value) {
//                         value = sign * std::stoi(std::string((*rit)[1]));
//                     }
//                     else if (std::is_floating_point<T>::value) {
//                         value = sign * std::stof(std::string((*rit)[1]));
//                     }
//                     else {

//                     }
//                 }
//                 else {
//                     value = sign * T(1);
//                 }
//                 if (value == T(0)) {
//                     continue;
//                 }
//                 if (std::string((*rit)[2]).length()) {
//                     degree = stoi(std::string((*rit)[2]));
//                 }
//                 else {
//                     if (std::string((*rit)[0]) != std::string((*rit)[1])) {
//                         degree = 1;
//                     }
//                     else {
//                         degree = 0;
//                     }
//                 }
//                 if (coefficients_.find(degree) != coefficients_.end()) {
//                     coefficients_[degree] += value;
//                 }
//                 else {
//                     coefficients_[degree] = value;
//                 }
//             }
//             std::cout << "\n HERE " << (*rit)[0] << ' ' << (*rit)[1] << ' ' <<    (*rit)[2] << "\n";
//             ++rit;
//         }
//    }


    // CPolynomial operator+(CPolynomial const& other) const {
    //     std::map<size_t, T> new_coefficients;
    //     for (size_t i = 0; i <= std::max(deg(), other.deg()); ++i) {
    //         auto this_finder = coefficients_.find(i);
    //         auto other_finder = other.coefficients_.find(i);
    //         if (this_finder != coefficients_.end() || other_finder != other.coefficients_.end()) {
    //             if (this_finder != coefficients_.end() && other_finder != other.coefficients_.end() \
    //             && this_finder->second != -(other_finder->second)) {
    //                 new_coefficients[i] = this_finder->second + other_finder->second;
    //             }
    //             else if (this_finder != coefficients_.end() && other_finder == other.coefficients_.end()) {
    //                 new_coefficients[i] = this_finder->second;
    //             }
    //             else if (this_finder == coefficients_.end() && other_finder != other.coefficients_.end()) {
    //                 new_coefficients[i] = other_finder->second;
    //             }
    //         }
    //     }
    //     return CPolynomial(new_coefficients);
    // }
    // CPolynomial operator-(CPolynomial const& other) {
    //     return CPolynomial(*this + (-other));
    // }