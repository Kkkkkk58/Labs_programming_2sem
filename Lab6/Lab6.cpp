#include <iostream>
#include "compiletime_polynomial.hpp"


int main()
{
    std::cout << "Hello World!\n";
    //std::cout << Degree<CTCEPolynomial<1, 2, 3, 4, 5, 6>>::value << "\n";
    constexpr int val = Degree<CTCEPolynomial<1, 2, 3, 4, 5, 6>>::value;
    constexpr int another = CTCEPolynomial<1, 2, 3>::evaluation(5);
    constexpr int one_more = CTCEPolynomial<1, 2, 3>::evaluation(-10);
    static_assert(one_more == 281);
    constexpr int another_temp = CTTPolynomial<5, 1, 2, 3>::value;
    constexpr int val_temp = Degree<CTTPolynomial<0, 1, 2, 3, 4, 5, 6>>::value;
    constexpr int one_more_temp = CTTPolynomial<-10, 1, 2, 3>::value;
    static_assert(another == another_temp);
    static_assert(val == val_temp);
    static_assert(one_more == one_more_temp);
    std::cout << val << " " << another << " " << one_more << "\n";
    CTTPolynomial<-10, 1, 2, 3> pol;
    CTCEPolynomial<1, 2, 3> pol_;

    std::cout << pol << "\n";
    std::cout << pol_;
}
