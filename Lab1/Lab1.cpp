#include "figures.hpp"
#include "polynomial.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>


void test_figures() {
    std::cout << "TESTING FIGURES IN PROGRESS...\n\n";
    CPolygonal_chain a({CPoint(1,2), CPoint(3,5)});
    std::cout << "Polyline length - " << a.length() << "\n";
    //std::cout << a[4] << "\n"; // ERR OUT_OF_RANGE
    CClosed_polygonal_chain b({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    CClosed_polygonal_chain c(b);
    std::cout << "Copy constructed from " << b << " is " << c << "\n";
    CPolygon k({CPoint(1,2), CPoint(3,5), CPoint(7, 2)});
    std::cout << "Polygon perimeter = " << k.perimeter() << ", area = " << k.area() << "\n";
    CTriangle f({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    CTriangle l = f;
    std::cout << "Assigned from " << f << " is " << l << "\n";
    std::vector<double> angles = l.angles();
    std::cout << "Angles: ";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << angles[i] << ' ';
    }
    std::cout << '\n';
    CRegular_polygon sq;
    std::cout << sq << " perimeter: " << sq.perimeter() << " area: " \
                << sq.area() << " side: " << sq.side() << '\n';
    CRegular_polygon oq = sq;
    CTrapezoid trap;
    std::cout << "Dynamic polymorphism: calling methods for base class pointers\n";
    std::vector<CPolygon *> polys_ptr{&k, &f, &sq, &trap};
    for (CPolygon const* it: polys_ptr) {
        std::cout << *it << " perimeter: " << it->perimeter() <<\
                    " area: " << it->area() << std::boolalpha << " is convex: " << it->is_convex() \
                    << " is regular: " << it->is_regular() << "\n";
        std::cout << "Sides: ";
        std::vector<double> sides_v = it->sides();
        for (size_t i = 0; i < it->size(); ++i) {
            std::cout << sides_v[i] << " ";
        }
        std::cout << "\nAngles: ";
        std::vector<double> angles_v = it->angles();
        for (size_t i = 0; i < it->size(); ++i) {
            std::cout << angles_v[i] << " ";
        }
        std::cout << "\n\n";
    }
    CTriangle tria{{0,3}, {-2,-3}, {-6,1}};
    std::cout << tria << " is " << tria.angle_type() << ", " << tria.side_type() << "\n\n";
    std::cout << "TESTING FIGURES DONE\n\n";
}

void test_polynomials() {
    std::cout << "TESTING POLYNOMIAL IN PROGRESS...\n\n";
    std::vector<int> coefs_vector{1, 98, 12, 45, 0, 0, 0, -54, 0, 0, 0};
    CPolynomial<int> from_vector(coefs_vector);
    std::cout << "Constructed from vector: " << from_vector << "\n";
    std::map<size_t, int> coefs_map{{0, 1}, {1, 98}, {2, 12}, {3, 45}, {4, 0}, {7, -54}, {98, 0}};
    CPolynomial<int> from_map(coefs_map);
    std::cout << "Constructed from map: " << from_map << "\n";
    std::string coefs_string("1 + 98    x + 12x ^2 + 45x^3 + 5x^5   -54x^7 + 0*x^4 + -5x^5");
    CPolynomial<int> from_string(coefs_string);
    std::cout << "Constructed from string: " << from_string << "\n";
    std::ifstream coefs_file("test.txt");
    CPolynomial<int> from_file;
    coefs_file >> from_file;
    std::cout << "Constucted from input: " << from_file << "\n";
    assert(("Listed polygons are equal", \
            from_string == from_map && from_map == from_vector && from_vector == from_file));
    assert(("Same checking", from_string <= from_vector && from_string >= from_vector));
    std::cout << "All of listed polynomials are equal\n";
    assert(("Multiplication, minus operations and plus work",
            +from_map - (-from_string) + from_file == 3 * from_vector));
    std::cout << "Multiplication, minus operations and plus work\n" << \
                from_map + from_string + from_file << " equals " << 3 * from_vector << "\n";
    from_vector *= 54;
    assert(("Divison by number works", from_vector / 54 == from_map));
    std::cout << "Division by number works\n";
    from_vector /= 54;
    std::cout << "Degree of polynomials is " << from_string.deg() << "\n";
    std::cout << "Basic polynomial of 1000-th degree is " << CPolynomial<int>(1000) << "\n";
    std::map<size_t, std::complex<float>> complex_map{{1, {12.54, 53.19}}, {15, {98.2, 2006.11}}};
    CPolynomial<std::complex<float>> complex_example(complex_map);
    std::cout << "Example of complex polynomial: " << complex_example << "\n";
    std::ifstream compl_input("complex.txt");
    CPolynomial<std::complex<float>> second_complex;
    compl_input >> second_complex;
    std::cout << "Complex from file: " << second_complex << "\n";
    assert(("They are equal", !(complex_example != second_complex)));
    CPolynomial<int> numerator({-42, 0, -12, 1});
    CPolynomial<int> denominator({-3, 1});
    assert(("Division works", numerator / denominator == CPolynomial<int>({-27, -9, 1}), \
            numerator % denominator == CPolynomial<int>({-123})));
    std::cout << numerator << " divided by " << denominator << " equals " << \
                numerator / denominator << ". Remainder is " << numerator % denominator << "\n";
    assert(("Division and multiplication work", \
            (numerator / denominator) * denominator + (numerator % denominator) == numerator));
    std::cout << "(" << numerator / denominator << ") * (" << denominator << ") + " \
                << numerator % denominator << " = " << numerator << "\n";
    CPolynomial<int> derivation({11, 45, 98, -12, 22, 56});
    assert(("Derivative is correct", \
            derivative(derivation, 3) == CPolynomial<int>({-72, 528, 3360})));
    std::cout << "Derivative of 3rd order from " << derivation << " is " \
                << derivative(derivation, 3) << "\n";
    CPolynomial<std::complex<int>> evaluated({{0, 7}, 3, 1, {5, 3}, 7, 4, 3});
    assert(("Evaluation is correct", \
            evaluated.evaluation({1, 1}) == std::complex<int>(-57, -24)));
    std::cout << "Evalution of " << evaluated << " in point (1,1) is " \
                << evaluated.evaluation({1, 1}) << "\n";
    std::cout << "TESTING POLYNOMIAL DONE\n\n";
}

int main() {
    test_figures();
    test_polynomials();
    std::cout << "\nTESTS PASSED SUCCESFULLY\n";
    return 0;
}