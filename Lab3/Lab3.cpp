#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <vector>
#include <string>
#include <memory_resource>
#include <random>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>
#include <array>
#include <thread>
#include "figures.hpp"
#include "polynomial.hpp"
#include "Cyclic_buffer.hpp"
#include "Algorithms.hpp"

bool ignore_case(char a, char b) {
	return (tolower(a) <= tolower(b));
}

template<typename T>
class Custom_deleter {
public:
	void operator()(T *value) {
		std::cout << "Custom_deleter here\n";
	}
};

void algo_test() {
	Cyclic_buffer<int> v(10, 2);
	
	std::partial_sum(v.cbegin(), v.cend(), v.begin());
	std::cout << "Among the numbers: ";
	std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << '\n';

	if (algorithms::all_of(v.cbegin(), v.cend(), [](int i) { return i % 2 == 0; })) {
		std::cout << "All numbers are even\n";
	}
	if (algorithms::none_of(v.cbegin(), v.cend(), std::bind(std::modulus<>(),
		std::placeholders::_1, 2))) {
		std::cout << "None of them are odd\n";
	}
	struct DivisibleBy
	{
		const int d;
		DivisibleBy(int n) : d(n) {}
		bool operator()(int n) const { return n % d == 0; }
	};

	if (algorithms::any_of(v.cbegin(), v.cend(), DivisibleBy(7))) {
		std::cout << "At least one number is divisible by 7\n";
	}

	if (algorithms::one_of(v.cbegin(), v.cend(), DivisibleBy(7))) {
		std::cout << "Exactly one number is divisible by 7\n";
	}

	int digits[] = { 3, 1, 4, 1, 5 };

	for (auto i : digits) std::cout << i << ' ';
	std::cout << ": is_sorted: " << std::boolalpha
		<< algorithms::is_sorted(std::begin(digits), std::end(digits)) << '\n';
	std::string boy("Boy");
	std::sort(std::begin(digits), std::end(digits));

	for (auto i : digits) std::cout << i << ' ';
	std::cout << ": is_sorted: "
		<< algorithms::is_sorted(std::begin(digits), std::end(digits), std::greater()) << '\n';
	std::cout << boy << " ";
	std::cout << ": is_sorted: " << std::boolalpha
		<< algorithms::is_sorted(std::begin(boy), std::end(boy), ignore_case) << '\n';
	
	Cyclic_buffer<int> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	auto is_even = [](int i) { return i % 2 == 0; };
	for (auto i : arr) std::cout << i << ' ';
	std::cout.setf(std::ios_base::boolalpha);
	std::cout << "is partitioned :" << algorithms::is_partitioned(arr.begin(), arr.end(), is_even) 
		<< "\n";

	std::partition(arr.begin(), arr.end(), is_even);
	for (auto i : arr) std::cout << i << ' ';
	std::cout << "is partitioned :" << algorithms::is_partitioned(arr.begin(), arr.end(), is_even) 
		<< "\n";

	std::reverse(arr.begin(), arr.end());
	for (auto i : arr) std::cout << i << ' ';
	std::cout << "is partitioned :" << algorithms::is_partitioned(arr.cbegin(), arr.cend(), is_even)
		<< "\n";

	std::vector<int> vec = { 1, 2, 1, 98 };
	for (auto i : vec) std::cout << i << ' ';
	auto it = algorithms::find_not(vec.begin(), vec.end(), 1);
	std::cout << "The first element that differs from 1 is " << *it << "\n";

	auto rit = algorithms::find_backward(vec.begin(), vec.end(), 1);
	std::cout << "Found 1 placed " << rit - vec.begin() << " : " << *rit << "\n";

	for (auto i : vec) std::cout << i << ' ';
	std::cout << " is palindrome: " << algorithms::is_palindrome(vec.begin(), vec.end()) << "\n";
	vec = { 1,2,2,1 };
	for (auto i : vec) std::cout << i << ' ';
	std::cout << " is palindrome: " << algorithms::is_palindrome(vec.begin(), vec.end()) << "\n";
	vec = { 1, 2, 3, 4, 5 };
	for (auto i : vec) std::cout << i << ' ';
	std::cout << " is palindrome by predicate \"is_even\": " << \
		algorithms::is_palindrome(vec.begin(), vec.end(), is_even) << "\n";
}


void buffer_test() {
	Cyclic_buffer<int> twelve = { 0,0,1,2,3,4,5,6,7, 0 };
	twelve.push_back(98);
	twelve.push_front(289374);
	twelve.replace(1, 5, 8);
	std::vector<int> te{ -28, -28 };

	twelve.shrink_to_fit();
	//twelve.insert(4, te.begin(), te.end());
	twelve.insert_extend(4, te.begin(), te.end());
	for (size_t i = 0; i < twelve.size(); ++i) {
		std::cout << i << " " << twelve[i] << "\n";
	}
	Cyclic_buffer<int> asldk = { 28,2903,1,3,5,6,7,78,0 };
	twelve.replace(twelve.begin() + 2, twelve.end(), asldk.begin(), asldk.end());
	std::cout << "================\n";

	for (auto iter = twelve.begin(); iter != twelve.end(); iter -= -1) {
		std::cout << *iter << "\n";
	}

	std::cout << "================\n";
	std::transform(twelve.begin(), twelve.end(), twelve.begin(), [](int a) { return a * 99; });
	auto rrit = twelve.end();
	do {
		--rrit;
		std::cout << *rrit << "\n";
	} while (rrit != twelve.begin());
	std::cout << "================\n";
	for (auto rit = twelve.rbegin(); rit != twelve.rend(); ++rit) {
		std::cout << *rit << "\n";
	}
	std::cout << "================\n";
	std::sort(twelve.begin(), twelve.end());
	auto rrrit = twelve.rend();
	do {
		--rrrit;
		std::cout << *rrrit << "\n";
	} while (rrrit != twelve.rbegin());
	std::cout << "================\n";
	std::cout << std::boolalpha << twelve.contains(0) << " " << (twelve == twelve) << "\n";
	std::cout << "================\n";
	Cyclic_buffer<CPolygonal_chain> polys(12, 5, CPolygonal_chain({ {1, 2}, {3,5}, {10,0} }));
	polys.shrink_to_fit();
	polys.push_back(CPolygonal_chain({ {1,1}, {9,8} }));
	for (auto const& c : polys) {
		std::cout << c << "\n";
	}
	std::cout << "================\n";
	std::array <CPolynomial<int>, 3> arr(CPolynomial<int>({1,2,5,7,98,2006}),
		CPolynomial<int>({ 58,337,228,7,-3,200 }),
		CPolynomial<int>({ 1,2 }));
	Cyclic_buffer<CPolynomial<int>, std::pmr::polymorphic_allocator<CPolynomial<int>>, \
		Custom_deleter<CPolynomial<int>>> polynomial_buff(arr.begin(), arr.end());
	polynomial_buff.push_back(CPolynomial<int>());
	for (auto pit = polynomial_buff.begin(); pit != polynomial_buff.end(); ++pit) {
		std::cout << "CPolynomial " << *pit << "\nDerivative of 2 order: " << derivative(*pit, 2) <<
			"\nEvaluation at 1: " << pit->evaluation(1) << "\n\n";
	}
}


int main() {
	std::cout << "Launching algorithms test...\n";
	algo_test();
	std::cout << "Algo test finished succesfully\n\nLaunching buffer test...\n";
	buffer_test();
	std::cout << "Buffer test finished succesfully\n\n";
	return 0;
}