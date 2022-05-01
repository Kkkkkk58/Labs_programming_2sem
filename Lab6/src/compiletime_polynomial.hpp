#ifndef COMPILETIME_POLYNOMIAL_HPP
#define COMPILETIME_POLYNOMIAL_HPP
#include <iostream>
//////template<int ...Args>
//////class ICTPolynomial {
//////public:
//////	virtual ~ICTPolynomial() {}
//////	virtual void print() = 0;
//////};
//////
//////template<int ...Args>
//////class CTPolynomial : ICTPolynomial {};

template<int ...Args>
class CTPolynomial;

template<int ...Args>
class Evaluation;


template<typename Polynomial>
class Degree {
public:
	static const int value = 1 + Degree<typename Polynomial::Shifted>::value;
};

template<>
class Degree<CTPolynomial<>> {
public:
	static const int value = -1;
};

template<>
class CTPolynomial<> {
public:
	using type = CTPolynomial<>;
	static consteval int evaluation(int) { return 0; }
	static consteval int degree() {
		return Degree<type>::value;
	}
};

template<int V>
class Evaluation<V> {
public:
	static const int value = 0;
};


template<int FirstElement, int... Rest>
class CTPolynomial<FirstElement, Rest...> {
public:
	using type = CTPolynomial<FirstElement, Rest...>;
	using Shifted = CTPolynomial<Rest...>;
	static consteval int degree() {
		return Degree<type>::value;
	}
	static consteval int evaluation(int a) {
		int eval = FirstElement;
		for (const auto coef : { Rest... }) {
			eval += coef * a;
			a *= a;
		}
		return eval;
	}
};


template<int V, int FirstElement, int... Rest>
class Evaluation<V, FirstElement, Rest...> {
public:
	static const int value = FirstElement + V * Evaluation<V, Rest...>::value;
};


std::ostream& operator<<(std::ostream& os, CTPolynomial<> const& poly) {
	os << 0;
	return os;
}

template<int FirstElement, int... Rest>
std::ostream& operator<<(std::ostream& os, CTPolynomial<FirstElement, Rest...> const& poly) {
	os << FirstElement;
	size_t degree = 1;
	for (const auto coef : { Rest... }) {
		if (coef != 0) {
			os << " + " << coef << "x";
		}
		if (degree != 1) {
			os << "^" << degree;
		}
		++degree;
	}
	return os;
}


#endif


