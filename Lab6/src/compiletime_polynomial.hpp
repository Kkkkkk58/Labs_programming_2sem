#ifndef COMPILETIME_POLYNOMIAL_HPP
#define COMPILETIME_POLYNOMIAL_HPP
#include <iostream>

template<int ...Args> 
class CTCEPolynomial;
template<int ...Args>
class CTTPolynomial;

template<typename Poly>
class Degree {
public:
	static const int value = 1 + Degree<typename Poly::Coefs>::value;
};

template<>
class CTCEPolynomial<> {
public:
	consteval int evaluation(int) { return 0; }
	//friend std::ostream& operator<<(std::ostream &os, CTCEPolynomial<> const& poly);
};
std::ostream& operator<<(std::ostream& os, CTCEPolynomial<> const& poly) {
	os << "Polynomial 0";
	return os;
}
template<>
class CTTPolynomial<> {
public:
	static const int value = 0;
};
template<int V>
class CTTPolynomial<V> {
public:
	static const int value = 0;
};

std::ostream& operator<<(std::ostream& os, CTTPolynomial<> const& poly) {
	os << "Polynomial 0";
	return os;
}

template<int V>
std::ostream& operator<<(std::ostream& os, CTTPolynomial<V> const& poly) {
	os << "Polynomial 0";
	return os;
}

template<>
class Degree<CTCEPolynomial<>> {
public:
	static const int value = -1;
};
template<int V>
class Degree<CTTPolynomial<V>> {
public:
	static const int value = -1;
};

template<int FirstElement>
class Degree<CTCEPolynomial<FirstElement>> {
public:
	static const int value = 0;
};
template<int V, int FirstElement>
class Degree<CTTPolynomial<V, FirstElement>> {
public:
	static const int value = 0;
};


template<int FirstElement, int... Rest>
class CTCEPolynomial<FirstElement, Rest...> {
public:
	static consteval int evaluation(int a) {
		int eval = FirstElement;
		for (const auto coef : { Rest... }) {
			eval += coef * a;
			a *= a;
		}
		return eval;
	}
	using Coefs = CTCEPolynomial<Rest...>;
};

template<int FirstElement, int... Rest>
std::ostream& operator<<(std::ostream& os, CTCEPolynomial<FirstElement, Rest...> const& poly) {
	os << "Polynomial " << FirstElement;
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

template<int V, int FirstElement, int... Rest>
class CTTPolynomial<V, FirstElement, Rest...> {
public:
	static const int value = FirstElement + V * CTTPolynomial<V, Rest...>::value;
	using Coefs = CTTPolynomial<V, Rest...>;
};

template<int V, int FirstElement, int... Rest>
std::ostream& operator<<(std::ostream& os, CTTPolynomial<V, FirstElement, Rest...> const& poly) {
	os << "Polynomial " << FirstElement;
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
	os << " evaluated at " << V << " is " << poly.value;
	return os;
}

#endif


