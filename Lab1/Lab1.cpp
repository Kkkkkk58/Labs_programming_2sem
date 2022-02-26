#include "figures.hpp"
#include "polynomial.hpp"
//#include <array>
#include "Complex.hpp"

int main() {
    // CPolygonal_chain a({CPoint(1,2), CPoint(3,5)});
    // std::cout << a.length() << "\n";
    // std::cout << a[4] << "\n";
    // CClosed_polygonal_chain b({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    // CClosed_polygonal_chain c(b);
    // std::cout << c.length() << "\n";
    // CPolygon k({CPoint(1,2), CPoint(3,5), CPoint(7, 2)});
    // std::cout << k.perimeter() << " " << k.area() << "\n";
    // //k.swap(c);
    // std::cout << k.area() << "\n";
    // CTriangle f({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    // CTriangle l = f;
    // std::cout << f.area() << "\n";
    // std::vector<double> angles = l.angles();
    // for (size_t i = 0; i < 3; ++i) {
    //     std::cout << angles[i] << ' ';
    // }
    // std::cout << '\n';
    // CRegular_polygon sq;
    // std::cout << sq.perimeter() << ' ' << sq.area() << '\n';
    // CTrapezoid trap;
    // std::vector<CPolygon> polys{k, f, sq, trap};
    // for (CPolygon const& it : polys) {
    //     std::cout << it.area() << ' ';
    // }
    // std::cout << "\n";
    // std::vector<CPolygon *> polys_ptr{&k, &f, &sq, &trap};
    // for (CPolygon const* it: polys_ptr) {
    //     std::cout << it->area() << ' ';
    // }
    // std::cout << '\n';
    // CPolygonal_chain bq({{1,2}, {2,5}, {2,5}, {2,5}, {1,2}});
    // std::cout << bq.size() << " " << bq[1] << "\n";
    // CTriangle pipa({CPoint(-1,2), CPoint(-1,5), CPoint(12,58)});
    // std::cout << pipa.size() << " " << pipa.perimeter() << " " << pipa.area() << " " << pipa.is_convex() << "\n";
    // CPolygon biba({{5,10}, {7,5}, {10,3}, {7,1}, {5,-5}, {3,1}, {0,3}, {3,5}});
    // std::cout << biba.perimeter() << " " << biba.area() << " " << biba.is_convex() << "\n";
    // CPolygon boba({{5,10}, {7,7}, {6,5}, {3,5}, {6,7}});
    // std::cout << boba.is_convex() << "\n";
    // CPolygon hihihi({{5,10}, {7,7}, {6,5}, {3,5}, {1,7}});
    // std::cout << hihihi.is_convex() << "\n";
    // std::vector<CPoint> vec = {CPoint(12,11), CPoint(12,111), CPoint(8712,19)};
    // CPolygon kkkk(vec);
    // //CPolygon h;
    // CTriangle tria({{0,3}, {-2,-3}, {-6,1}});
    // std::cout << tria.angle_type() << " " << tria.side_type();
    // std::array<CPoint, 3> arr = {CPoint(12,11), CPoint(12,111), CPoint(8712,19)};
    // //CPolygon koko(arr);
    //CTrapezoid area({2,5}, {6,5}, {3,0}, {5,0});
    // std::cout << area.size() << "\n";
    // std::cout << area.area();
    //CTriangle kk{{1,1}};
    //CRegular_polygon asl({{1,12},{1,15},{12,11111},{10,0}});
    //std::cout << asl.is_regular();
    // CClosed_polygonal_chain v({{12,10},{15,27}, {98,2222}});
    // CPolygonal_chain *ooo = &v;
    // std::cout << *ooo;
    //CPolygon a({{-3,4},{1,2},{2,-1},{-1,-3},{-4,-1},{-4,2}});
    // CTrapezoid example({1,1}, {10,1}, {8,6}, {5,6});
    // std::cout << example.area() << "\n" << example << "\n";
    // std::cout << "HI";

    {
        std::vector<double> coefs{1, 12};
        CPolynomial<double> x(coefs);
        std::cout << x <<"\n";
        std::vector<Complex> ocoefs{{1,0}, {98,2006}, {}, {0,0}};
        CPolynomial<Complex> y(ocoefs);
        std::cout << y << "\n";
        std::cout << y.deg() << "\n";
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
        std::map<size_t, Complex> cmap{{1,{12,53}}, {15, {98, 2006}}};
        CPolynomial<Complex> cm(cmap);
        std::cout << cm.deg() << "\n";
        CPolynomial<double> yy;
        std::cout << yy << " " << yy.deg() << "\n";
        std::cout << cm << "\n";
        CPolynomial<double> var;
        std::cout << var << "\n";
        std::ofstream fout("log");
        fout << cm << "\n";
        // Добавить в енаблеиф
        // std::map<size_t, std::complex<double>> complmap{{1, {12,53}}, {15,{98,2006}}};
        // CPolynomial<std::complex<double>> com(complmap);
        // std::cout << com;
        std::cout << "\nHI";
    }
    return 0;
}