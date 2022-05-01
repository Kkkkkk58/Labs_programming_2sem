#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

TEST(BasicTest, always_true) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(PolynomialEvaluation, ResultComparison) {
    constexpr int val = Evaluation<1, 1, 2, 3, 4, 5>::value;
    CTPolynomial<1, 2, 3, 4, 5> poly;
    static_assert(val == poly.evaluation(1));
    ASSERT_EQ(val, poly.evaluation(1));
}

TEST(PolynomialEvaluation, MyPolynomialComparison) {
    CPolynomial<int> poly({ 98, 2006, 456 });
    int pt = poly.evaluation(6);
    constexpr int ct = CTPolynomial<98, 2006, 456>::evaluation(6);
    constexpr int tt = Evaluation<6, 98, 2006, 456>::value;
    ASSERT_EQ(pt, ct);
    ASSERT_EQ(ct, tt);
}


TEST(PolynomialOutput, StringsComparison) {
    CPolynomial<int> poly({ 98, 2006, 456 });
    CTPolynomial<98, 2006, 456> ppoly;
    std::stringstream res_1, res_2;
    res_1 << poly;
    res_2 << ppoly;
    std::cout << ppoly << "\n" << poly << "\n";
    ASSERT_EQ(res_1.str(), res_2.str());
}


TEST(PolynomialEvaluation, EmptyInitialized) {
    CTPolynomial All_my_plans_fell_through_my_hand_They_fell_through_my_hands_on_me;
    Degree<CTPolynomial<>> All_my_dreams_it_suddenly_seems_It_suddenly_seems_empty;
    CPolynomial<int> Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i;
    ASSERT_EQ(All_my_plans_fell_through_my_hand_They_fell_through_my_hands_on_me.degree(), \
        All_my_dreams_it_suddenly_seems_It_suddenly_seems_empty.value);
    ASSERT_EQ(Empty_i_i_i_i_i_i_Empty_i_i_i_i_i_i_i_i.deg(), -1);
}

//TEST(TestCaseName, TestName) {
//  EXPECT_EQ(1, 1);
//  EXPECT_TRUE(true);
//}
//
//
//TEST(case_1, test_1) {
//    std::cout << "Hello World!\n";
//    //std::cout << Degree<CTCEPolynomial<1, 2, 3, 4, 5, 6>>::value << "\n";
//    constexpr int val = Degree<CTCEPolynomial<1, 2, 3, 4, 5, 6>>::value;
//    constexpr int another = CTCEPolynomial<1, 2, 3>::evaluation(5);
//    constexpr int one_more = CTCEPolynomial<1, 2, 3>::evaluation(-10);
//    //static_assert(one_more == 281);
//    constexpr int another_temp = CTTPolynomial<5, 1, 2, 3>::value;
//    constexpr int val_temp = Degree<CTTPolynomial<0, 1, 2, 3, 4, 5, 6>>::value;
//    std::cout << CTTPolynomial<-10, 1, 2, 3>::value << "\n";
//    constexpr int one_more_temp = CTTPolynomial<-10, 1, 2, 3>::value;
//    //static_assert(std::is_constant_evaluated());
//
//    ASSERT_EQ(val, val_temp);
//    ASSERT_EQ(another, another_temp);
//    ASSERT_EQ(one_more, one_more_temp);
//    //static_assert(another == another_temp);
//    //static_assert(val == val_temp);
//    //static_assert(one_more == one_more_temp);
//    //std::cout << val << " " << another << " " << one_more << "\n";
//    CTTPolynomial<-10, 1, 2, 3> pol;
//    CTCEPolynomial<1, 2, 3> pol_;
//    std::cout << pol << "\n";
//    std::cout << pol_ << "\n";
//}
//////TEST(test_case_1, test_1) {
//////    std::cout << Degree<CTPolynomial<1>>::value << "\n";
//////}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}