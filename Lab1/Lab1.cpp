#include "figures.hpp"
#include "polynomial.hpp"
#include <array>
int main() {
    Polygonal_chain a({CPoint(1,2), CPoint(3,5)});
    std::cout << a.length() << "\n";
    std::cout << a[4] << "\n";
    Closed_polygonal_chain b({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    Closed_polygonal_chain c(b);
    std::cout << c.length() << "\n";
    Polygon k({CPoint(1,2), CPoint(3,5), CPoint(7, 2)});
    std::cout << k.perimeter() << " " << k.area() << "\n";
    //k.swap(c);
    std::cout << k.area() << "\n";
    Triangle f({CPoint(1,2), CPoint(3,5), CPoint(7,-2)});
    Triangle l = f;
    std::cout << f.area() << "\n";
    std::vector<double> angles = l.angles();
    for (size_t i = 0; i < 3; ++i) {
        std::cout << angles[i] << ' ';
    }
    std::cout << '\n';
    Regular_polygon sq;
    std::cout << sq.perimeter() << ' ' << sq.area() << '\n';
    Trapezoid trap;
    std::vector<Polygon> polys{k, f, sq, trap};
    for (Polygon const& it : polys) {
        std::cout << it.area() << ' ';
    }
    std::cout << "\n";
    std::vector<Polygon *> polys_ptr{&k, &f, &sq, &trap};
    for (Polygon const* it: polys_ptr) {
        std::cout << it->area() << ' ';
    }
    std::cout << '\n';
    Polygonal_chain bq({{1,2}, {2,5}, {2,5}, {2,5}, {1,2}});
    std::cout << bq.size() << " " << bq[1] << "\n";
    Triangle pipa({CPoint(-1,2), CPoint(-1,5), CPoint(12,58)});
    std::cout << pipa.size() << " " << pipa.perimeter() << " " << pipa.area() << " " << pipa.is_convex() << "\n";
    Polygon biba({{5,10}, {7,5}, {10,3}, {7,1}, {5,-5}, {3,1}, {0,3}, {3,5}});
    std::cout << biba.perimeter() << " " << biba.area() << " " << biba.is_convex() << "\n";
    Polygon boba({{5,10}, {7,7}, {6,5}, {3,5}, {6,7}});
    std::cout << boba.is_convex() << "\n";
    Polygon hihihi({{5,10}, {7,7}, {6,5}, {3,5}, {1,7}});
    std::cout << hihihi.is_convex() << "\n";
    std::vector<CPoint> vec = {CPoint(12,11), CPoint(12,111), CPoint(8712,19)};
    Polygon kkkk(vec);
    //Polygon h;
    Triangle tria({{0,3}, {-2,-3}, {-6,1}});
    std::cout << tria.angle_type() << " " << tria.side_type();
    std::array<CPoint, 3> arr = {CPoint(12,11), CPoint(12,111), CPoint(8712,19)};
    //Polygon koko(arr);
    return 0;
}