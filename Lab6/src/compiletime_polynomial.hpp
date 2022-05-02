#ifndef COMPILETIME_POLYNOMIAL_HPP
#define COMPILETIME_POLYNOMIAL_HPP
#include <iostream>
#include "polynomial.hpp"

// Объявление шаблонного класса полинома
template<int ...Args>
class CTPolynomial;

// Объявление метафункции, рассчитывающей значение полинома в точке
template<int V, typename Polynomial>
class Evaluation;

// Объявление метафункции, рассчитывающей значение производной от функции, заданной полиномом, в точке
template<int Value, int Degree, typename Polynomial>
class Derivative;

// Объявление метафункции, рассчитывающей значение определённого интеграла
// от функции, заданной полиномом, в соответствии с заданными пределами интегрирования
template<int LowerBound, int UpperBound, int Degree, typename Polynomial>
class Integral;

// Метафункция, рассчитывающая степень полинома
template<typename Polynomial>
class Degree {
public:
	static const int value = 1 + Degree<typename Polynomial::Shifted>::value;
};

// Частная специализация шаблонного класса Degree - база для случая пустого полинома
template<>
class Degree<CTPolynomial<>> {
public:
	// Степень пустого полинома = -1
	static const int value = -1;
};


// Частная специализация класса полинома для случая без коэффициентов
template<>
class CTPolynomial<> {
public:
	using type = CTPolynomial<>;
	static consteval long long evaluation(int) { return 0; }
	static consteval int degree() { return -1; }
	static consteval long long derivative(int value) { return 0; }
	static consteval double integral(int, int) { return 0; }
	static void print(std::ostream& os, int deg = -1, bool is_first_element = true) { 
		if (deg == -1) {
			os << 0;
		}
	}
	operator CPolynomial<int>() const {
		return CPolynomial<int>();
	}
};

// Частная специализация класса Evaluation для случая пустого полинома - база рекурсии
template<int V>
class Evaluation<V, CTPolynomial<>> {
public:
	static const long long value = 0;
};

// Частная специализация класса Derivative для случая пустого полинома - база рекурсии
template<int V, int Degree>
class Derivative<V, Degree, CTPolynomial<>> {
public:
	static const long long value = 0;
};

// Частная специализация класса Integral для случая пустого полинома - база рекурсии
template<int LowerBound, int UpperBound, int Degree>
class Integral<LowerBound, UpperBound, Degree, CTPolynomial<>> {
public:
	inline static const double value = 0;
};

// Частная специализация класса CTPolynomial для полинома с коэффициентами
template<int FirstElement, int... Rest>
class CTPolynomial<FirstElement, Rest...> {
public:
	using type = CTPolynomial<FirstElement, Rest...>;
	using Shifted = CTPolynomial<Rest...>;
	static consteval int degree() { return sizeof...(Rest); }
	static consteval long long derivative(int value) {
		if constexpr (sizeof...(Rest) == 0) {
			return 0;
		}
		else {
			int eval = 0;
			int degree = 1;
			int value_mult = value;
			for (auto const coef : { Rest... }) {
				if (degree == 1) {
					eval = coef;
				}
				else {
					eval += coef * value * degree;
					value *= value_mult;
				}
				++degree;
			}
			return eval;
		}
	}
	static consteval long long evaluation(int a) {
		if constexpr (sizeof...(Rest) == 0) {
			return FirstElement;
		}
		else {
			int eval = FirstElement;
			for (const auto coef : { Rest... }) {
				eval += coef * a;
				a *= a;
			}
			return eval;
		}
	}
	static consteval double integral(int lower_bound, int upper_bound) {
		double result = (upper_bound - lower_bound) * FirstElement;
		if constexpr (sizeof...(Rest) == 0) {
			return result;
		}
		else {
			size_t degree = 2;
			int upper_mult = upper_bound, lower_mult = lower_bound;
			for (const auto coef : { Rest... }) {
				upper_bound *= upper_mult;
				lower_bound *= lower_mult;
				result += (upper_bound - lower_bound) * static_cast<double>(coef) / degree;
				++degree;
			}
			return result;
		}
	}
	static void print(std::ostream &os, int deg = degree(), bool is_first_element = true) {
		int relative_degree = deg - degree();
		if (FirstElement != 0) {
			if (relative_degree == 0) {
				os << FirstElement;
				is_first_element = false;
			}
			else {
				if (is_first_element) {
					is_first_element = false;
				}
				else {
					os << " + ";
				}
				os << FirstElement << "x";
			}
			if (relative_degree > 1) {
				os << "^" << relative_degree;
			}
		}
		Shifted::print(os, deg, is_first_element);
	}
	operator CPolynomial<int>() const {
		return CPolynomial<int>{ FirstElement, Rest... };
	}
};

// Частная специализация класса Evaluation для полинома с коэффициентами
template<int V, int FirstElement, int... Rest>
class Evaluation<V, CTPolynomial<FirstElement, Rest...>> {
public:
	static const long long value = FirstElement + V * Evaluation<V, CTPolynomial<Rest...>>::value;
};

// Частная специализация класса Derivative для полинома с коэффициентами
template<int V, int Degree, int FirstElement, int... Rest>
class Derivative<V, Degree, CTPolynomial<FirstElement, Rest...>> {
public:
	static const long long value = (Degree - sizeof...(Rest)) * FirstElement \
		+ V * Derivative<V, Degree, CTPolynomial<Rest...>>::value;
};

// Метафункция Pow для возведения числа в неотрицательную степень
template<int Value, int Degree>
class Pow {
public:
	static const long long value = Value * Pow<Value, Degree - 1>::value;
};

// Частная специализация класса Pow для возведения в нулевую степень - база рекурсии
template<int Value>
class Pow<Value, 0> {
public:
	static const long long value = 1;
};

// Частная специализация класса Integral для полинома с коэффициентами
template<int LowerBound, int UpperBound, int Degree, int FirstElement, int... Rest>
class Integral<LowerBound, UpperBound, Degree, CTPolynomial<FirstElement, Rest...>> {
public:
	inline static const double value = static_cast<double>(FirstElement) / (Degree - sizeof...(Rest) + 1) * \
		(Pow<UpperBound, (Degree - sizeof...(Rest) + 1)>::value  - Pow<LowerBound, (Degree - sizeof...(Rest) + 1)>::value) + \
		Integral<LowerBound, UpperBound, Degree, CTPolynomial<Rest...>>::value;
};

// Перегрузка оператора вывода в поток полинома
template<int... Args>
std::ostream& operator<<(std::ostream& os, CTPolynomial<Args...> const& poly) {
	poly.print(os);
	return os;
}

// Внешняя функция вывода в поток для полинома
template<int... Args>
void print(std::ostream& os, CTPolynomial<Args...> const& poly) {
	if constexpr (sizeof...(Args) == 0) {
		os << 0;
	}
	else {
		size_t degree = 0;
		bool is_first_element = true;
		for (const auto coef : { Args... }) {
			if (coef != 0) {
				if (degree != 0) {
					if (is_first_element) {
						is_first_element = false;
					}
					else {
						os << " + ";
					}
					os << coef << "x";
				}
				else {
					is_first_element = false;
					os << coef;
				}
				if (degree > 1) {
					os << "^" << degree;
				}
			}
			++degree;
		}
	}
}


#endif


