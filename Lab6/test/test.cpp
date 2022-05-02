#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>


TEST(PolynomialInitialization, EmptyInitialized) {
    CPolynomial<int> All_my_plans_fell_through_my_hand_They_fell_through_my_hands_on_me;
    Degree<CTPolynomial<>> All_my_dreams_it_suddenly_seems_It_suddenly_seems_empty;
    CTPolynomial Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i;
    EXPECT_EQ(Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i.degree(), \
        All_my_dreams_it_suddenly_seems_It_suddenly_seems_empty.value);
    EXPECT_EQ(All_my_plans_fell_through_my_hand_They_fell_through_my_hands_on_me.deg(), -1);
    EXPECT_EQ(Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i.evaluation(98), 0);
    EXPECT_EQ((Evaluation<98, CTPolynomial<>>::value), 0);
    EXPECT_EQ(Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i.derivative(2006), 0);
    EXPECT_EQ((Derivative<2006, -1, CTPolynomial<>>::value), 0);
    EXPECT_EQ(Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i.integral(0, 2), 0);
    EXPECT_EQ((Integral<0, 2, -1, CTPolynomial<>>::value), 0);
    std::stringstream res_1, res_2, res_3;
    res_1 << Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i;
    res_2 << All_my_plans_fell_through_my_hand_They_fell_through_my_hands_on_me;
    print(res_3, Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i);
    EXPECT_EQ(res_1.str(), res_2.str());
    EXPECT_EQ(res_2.str(), res_3.str());
    EXPECT_EQ(res_3.str(), "0");
}


TEST(PolynomialEvaluation, ConstExprAndTemplateResultComparison) {
    constexpr int val = Evaluation<1, CTPolynomial<1, 2, 3, 4, 5>>::value;
    CTPolynomial<1, 2, 3, 4, 5> poly;
    static_assert(val == poly.evaluation(1));
    EXPECT_EQ(val, poly.evaluation(1));
}


TEST(PolynomialEvaluation, CPolynomialEvaluationComparison) {
    CPolynomial<int> poly({ 98, 2006, 456 });
    long long pt = poly.evaluation(6);
    constexpr long long ct = CTPolynomial<98, 2006, 456>::evaluation(6);
    constexpr long long tt = Evaluation<6, CTPolynomial<98, 2006, 456>>::value;
    EXPECT_EQ(pt, ct);
    EXPECT_EQ(ct, tt);
}


TEST(PolynomialDegree, ConstExprAndTemplateDegreeComparison) {
    CTPolynomial<98, 3, 9, 1, 2, 0, 0, 0, 9> poly;
    EXPECT_EQ(poly.degree(), Degree<decltype(poly)>::value);
}


TEST(PolynomialDegree, CPolynomialDegreeComparison) {
    CTPolynomial<98, 3, 9, 1, 2, 0, 0, 0, 0, 0, 0, 19> poly;
    CPolynomial<int> cpoly("98 + 3x + 9x^2 + x^3 + 2x^4 + 19x^11");
    EXPECT_EQ(poly.degree(), cpoly.deg());
    EXPECT_EQ(cpoly.deg(), 11);
}


TEST(PolynomialDerivative, ConstExprAndTemplateDerivativeComparison) {
    CTPolynomial<1, 2, 3, 4, 5> coefs_filled_poly;
    CTPolynomial<19> single_coef_poly;
    EXPECT_EQ(coefs_filled_poly.derivative(1), (Derivative<1, Degree<decltype(coefs_filled_poly)>::value, decltype(coefs_filled_poly)>::value));
    EXPECT_EQ(single_coef_poly.derivative(18239), (Derivative<12, 0, decltype(single_coef_poly)>::value));
}


TEST(PolynomialDerivative, CPolynomialDerivativeComparison) {
    CTPolynomial<1, 2, 3, 4, 5> coefs_filled_poly;
    CPolynomial<int> ccoefs("1 + 2x + 3x^2 + 4x^3 + 5x^4");
    CPolynomial<int> csingle({ 19 });
    EXPECT_EQ(coefs_filled_poly.derivative(25), derivative(ccoefs, 1).evaluation(25));
    EXPECT_EQ((Derivative<55, 0, CTPolynomial<19>>::value), derivative(csingle, 1).evaluation(55));
}


TEST(PolynomialIntegral, IntegralEvaluationCheck) {
    EXPECT_TRUE((Integral<11, 12, 3, CTPolynomial<1, 2, 5, 7>>::value - CTPolynomial<1, 2, 5, 7>::integral(11, 12)) < EPS);
    EXPECT_TRUE((CTPolynomial<1, 2, 5, 7>::integral(11, 12) - 11351.916666666) < EPS);
}


TEST(PolynomialOutput, StringStreamsComparison) {
    CPolynomial<int> poly({ -98, 2006, 0, 0, 456 });
    CTPolynomial<-98, 2006, 0, 0, 456> ppoly;
    std::stringstream res_1, res_2, res_3;
    res_1 << poly;
    res_2 << ppoly;
    print(res_3, ppoly);
    std::cout << ppoly << "\n" << poly << "\n";
    print(std::cout, ppoly);
    std::cout << "\n";
    EXPECT_EQ(res_1.str(), res_2.str());
    EXPECT_EQ(res_2.str(), res_3.str());
    EXPECT_EQ(res_1.str(), "-98 + 2006x + 456x^4");
}


TEST(PolynomialConversion, ConversionTest) {
    CTPolynomial<1, 2, 0, 3, 4, 0, 12, 87> poly;
    CPolynomial<int> cpoly = poly;
    EXPECT_TRUE(cpoly == poly);
    EXPECT_EQ(poly.degree(), cpoly.deg());
    EXPECT_EQ((Evaluation<11, decltype(poly)>::value), cpoly.evaluation(11));
    EXPECT_EQ(poly.derivative(4), derivative(cpoly, 1).evaluation(4));
    EXPECT_TRUE((Integral<0, 2, 7, decltype(poly)>::value - 3047.03) < EPS);
    std::stringstream res_1, res_2;
    res_1 << poly;
    res_2 << cpoly;
    EXPECT_EQ(res_1.str(), res_2.str());
}


int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}