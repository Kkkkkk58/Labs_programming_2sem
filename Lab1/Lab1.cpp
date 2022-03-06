#include "figures.hpp"
#include "polynomial.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

int main() {
    CPolygonal_chain a({CPoint(1,2), CPoint(3,5)});
    std::cout << a.length() << "\n";
    //std::cout << a[4] << "\n"; //ERR
    CClosed_polygonal_chain b({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    CClosed_polygonal_chain c(b);
    std::cout << c.length() << "\n";
    CPolygon k({CPoint(1,2), CPoint(3,5), CPoint(7, 2)});
    std::cout << k.perimeter() << " " << k.area() << "\n";
    b.swap(c);
    std::cout << k.area() << "\n";
    CTriangle f({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    CTriangle l = f;
    std::cout << f.area() << "\n";
    std::vector<double> angles = l.angles();
    for (size_t i = 0; i < 3; ++i) {
        std::cout << angles[i] << ' ';
    }
    std::cout << '\n';
    CRegular_polygon sq;
    std::cout << sq << " " << sq.perimeter() << ' ' << sq.area() << " " << sq.side() << '\n';
    CRegular_polygon oq = sq;
    std::cout << "OQ" << oq << "\n";
    CTrapezoid trap;
    std::vector<CPolygon> polys{k, f, sq, trap};
    for (CPolygon const& it : polys) {
        std::cout << it.area() << ' ';
    }
    std::cout << "\n";
    std::vector<CPolygon *> polys_ptr{&k, &f, &sq, &trap};
    for (CPolygon const* it: polys_ptr) {
        std::cout << it->area() << ' ';
    }
    std::cout << '\n';
    CPolygonal_chain bq({{1,2}, {2,5}, {2,5}, {2,5}, {1,2}});
    std::cout << bq.size() << " " << bq[1] << "\n";
    CTriangle pipa({CPoint(-1,2), CPoint(-1,5), CPoint(12,58)});
    std::cout << pipa.size() << " " << pipa.perimeter() << " " << pipa.area() << " " << pipa.is_convex() << "\n";
    CPolygon biba({{5,10}, {7,5}, {10,3}, {7,1}, {5,-5}, {3,1}, {0,3}, {3,5}});
    std::cout << biba.perimeter() << " " << biba.area() << " " << biba.is_convex() << "\n";
    CPolygon boba({{5,10}, {7,7}, {6,5}, {3,5}, {6,7}});
    std::cout << boba.is_convex() << "\n";
    CPolygon hihihi({{5,10}, {7,7}, {6,5}, {3,5}, {1,7}});
    std::cout << hihihi.is_convex() << "\n";
    std::vector<CPoint> vec = {CPoint(12,11), CPoint(12,111), CPoint(8712,19)};
    CPolygon kkkk(vec);
    CPolygon h;
    CTriangle tria{{0,3}, {-2,-3}, {-6,1}};
    std::cout << tria.angle_type() << " " << tria.side_type();
    //CTrapezoid area({2,5}, {6,5}, {3,0}, {5,0});  // SELF INTERSECTS
    // std::cout << area.size() << "\n";
    // std::cout << area.area();
    //CTriangle kk{{1,1}};    //INVALID TRIANGLE
    //CRegular_polygon asl({{1,12},{1,15},{12,11111},{10,0}});    //IRREGULAR
    //std::cout << asl.is_regular();
    CClosed_polygonal_chain v({{12,10},{15,27}, {98,2222}});
    CPolygonal_chain *ooo = &v;
    std::cout << *ooo;
    CPolygon ahahahh({{-3,4},{1,2},{2,-1},{-1,-3},{-4,-1},{-4,2}});
    CTrapezoid example({1,1}, {10,1}, {8,6}, {5,6});
    std::cout << example.area() << "\n" << example << "\n";
    std::cout << "HI\n";

    {
        std::vector<double> coefs{1, 12};
        CPolynomial<double> x(coefs);
        std::cout << x <<"\n";
        std::vector<std::complex<int>> ocoefs{{1,0}, {98,2006}, {}, {0,0}};
        CPolynomial<std::complex<int>> y(ocoefs);
        std::cout << y << "\n";
        std::cout << y.deg() << "\n";
        CPolynomial<double> new_x{1, 98};
        CPolynomial<double> damn = x + new_x;
        std:: cout << damn << " " << x << "\n";
        CPolynomial<double> kkk = x;
        std::cout << kkk << "\n";
        x += kkk;
        std::cout << x << "\n";
        std::cout << x / 2 << "\n";
        x /= 2;
        std::cout << x << "\n";
        x *= 2;
        std::cout << x << "\n";
        std::cout << x - 2 * kkk << "\n";
        CPolynomial<int> a({1, 1, 1, 1});
        CPolynomial<int> b(4, 1);
        std::cout << (a == b) << " " << (a != b) << "\n";
        std::cout<< a[5] << "\n";
        std::map<size_t, std::complex<int>> cmap{{1,{12,53}}, {15, {98, 2006}}};
        CPolynomial<std::complex<int>> cm(cmap);
        std::cout << cm.deg() << "\n";
        CPolynomial<double> yy;
        std::cout << yy << " " << yy.deg() << "\n";
        std::cout << cm << "\n";
        CPolynomial<double> var;
        std::cout << var << "\n";
        std::ofstream fout("log");
        fout << cm << "\n";
        std::map<size_t, std::complex<double>> complmap{{1, {12,53}}, {15,{98,2006}}, {63, {0,0}}};
        CPolynomial<std::complex<double>> com(complmap);
        std::cout << com << "\n";
        CPolynomial<float> inputted;
        std::ifstream fin("test.txt");
        fin >> inputted;
        CPolynomial<float> other({12, 56, -48, 56, 98});
        //other *= derivative(other, 2);
        std::cout << other * other << "HERE \n";
        CPolynomial<int> numerator({-4, 0, -2, 1});
        CPolynomial<int> denominator({-3, 1});
        CPolynomial<int> denominator1(numerator);
        std::cout << "CRINGE\n";
        std::cout << numerator / 2 << "\n";
        std::cout << CPolynomial<int>({12, 98, 45, 50}) + CPolynomial<int>({-1, -98, 100, -50}) << "\n";
        std::cout << numerator % denominator << "\n";
        std::cout << "\n WOW" << inputted << "\n";
        std::cout << (inputted == other) << (inputted  < other) << (inputted > other) << \
        (inputted != other) << (inputted <= other) << (inputted >= other);
        std::string st(" (11.45,98.15) + x + -(100,56)x ^2");
        CPolynomial<std::complex<double>> str_tr(st);
        std::cout << str_tr;
        std::cout << "\nHI\n";
        std::cout << other.evaluation(1) << "\n";
        std::cout << derivative(other, 2) << "\n";
        CPolynomial<double> basic({1, 1});
        std::cout << derivative(basic, 1) << " " << derivative(basic, 2) << "\n";
        CPolynomial<std::complex<int>> wow({{12, 48}, {54, 11}, {4, 4}});
        std::cout << wow << "\n";
        std::cout << derivative(wow, 1) << "\n";
        CPolynomial<int> extra(5, 43);
        std::cout << "LESSSSS " << (extra < numerator) << "\n";
        std::cout << extra << "\n";
        std::cout << derivative(CPolynomial<int>({1,1}),1) << "\n";
        //CPolynomial<unsigned> dddddd{{12,48}};
    }
    CClosed_polygonal_chain salad({{1,1}, {2,2}, {98, 45}, {-1,-1}, {0,0}, {1,1}});
    std::cout << salad << "\n";
    CPolynomial<int> sdlkf("12x^3 + 12x^2 - 12x^2 + 98x^3 - 55 x  + 12 + 95*x - 98x^3");
    std::cout << sdlkf;
    return 0;
}