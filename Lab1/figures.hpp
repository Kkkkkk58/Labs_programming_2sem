#ifndef FIGURES_HPP
#define FIGURES_HPP
#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include <cmath>
#include <memory>
#include <typeinfo>
#include <utility>
#include <fstream>


class CPoint {
public:
    CPoint(double const& x = 0, double const& y = 0);
    CPoint(CPoint const&);
    CPoint(CPoint &&);
    CPoint &operator=(CPoint const&);
    CPoint &operator=(CPoint &&);
    CPoint operator+(CPoint const&) const;
    CPoint operator-() const;
    CPoint operator-(CPoint const&) const;
    CPoint &operator+=(CPoint const&);
    CPoint &operator-=(CPoint const&);
    CPoint &operator++();
    CPoint operator++(int);
    double operator[](size_t) const;
    bool operator==(CPoint const&) const;
    bool operator!=(CPoint const&) const;
    void swap(CPoint &);
    ~CPoint() = default;
    double x() const;
    double y() const;
private:
    double x_;
    double y_;
};

std::ostream& operator<<(std::ostream &, CPoint const&);

class ILine {
public:
    ILine() {}
    virtual double length() const = 0;
    virtual ~ILine() {};
};

class CVector : public ILine {
public:
    CVector(CPoint const& start, CPoint const& end) : start_(start), end_(end) {}
    ~CVector() = default;
    CVector(CVector const& other) : start_(other.start_), end_(other.end_) {}
    CVector &operator=(CVector const& other) {
        if (this != &other) {
            start_ = other.start_;
            end_ = other.end_;
        }
        return *this;
    }
    double x() const {
        return end_.x() - start_.x();
    }
    double y() const {
        return end_.y() - start_.y();
    }
    double length() const {
        return std::sqrt(x() * x() + y() * y());
    }
    double operator*(CVector const& other) {
        return x() * other.x() + y() * other.y();
    }
    friend double cross_prod_val(CVector const& a, CVector const& b) {
        //first_side.x() * second_side.y() - second_side.x() * first_side.y()
        return a.x() * b.y() - b.x() * a.y();
    }
private:
    CPoint start_;
    CPoint end_;
};


class CPolygonal_chain : public ILine {
public:
    CPolygonal_chain(std::vector<CPoint> const& vertices = std::vector<CPoint>()) 
    : vertices_(std::vector<CPoint>(vertices)) {
        if (vertices_.size() > 1) {
            for (auto adj_finder = std::adjacent_find(vertices_.begin(), vertices_.end());\
            adj_finder != vertices_.end(); ) {
                adj_finder = vertices_.erase(adj_finder);
                adj_finder = std::adjacent_find(adj_finder, vertices_.end());
            }
        }
        if (vertices_.size() >= 3) {
            for (auto third_vertex = vertices_.begin() + 2; third_vertex != vertices_.end(); ) {
                auto first_vertex = third_vertex - 2;
                auto second_vertex = third_vertex - 1;
                if (((third_vertex->x() - first_vertex->x()) * (second_vertex->y() - first_vertex->y())) \
                == ((third_vertex->y() - first_vertex->y()) * (second_vertex->x() - first_vertex->x()))) {
                    third_vertex = vertices_.erase(third_vertex);
                }
                else {
                    ++third_vertex;
                }
                
            }
        }
    }
    virtual size_t size() const { return vertices_.size(); }
    CPolygonal_chain(CPolygonal_chain const& other) 
    : vertices_(other.vertices_) {}
    CPolygonal_chain &operator=(CPolygonal_chain const& other) {
        if (this != &other) {
            vertices_ = other.vertices_;
        }
        return *this;
    }
    CPolygonal_chain(CPolygonal_chain && other) {
        swap(other);
    }
    CPolygonal_chain &operator=(CPolygonal_chain && other) {
        swap(other);
        return *this;
    }
    void swap(CPolygonal_chain &other) { std::swap(vertices_, other.vertices_); }
    ~CPolygonal_chain() = default;
    virtual double length() const override {
        double length = 0;
        if (size() > 1) {
            auto iter = vertices_.begin();
            double first_x = iter->x();
            double first_y = iter->y();
            while (++iter != vertices_.end()) {
                double second_x = iter->x();
                double second_y = iter->y();
                length += std::sqrt(std::pow((second_x - first_x), 2) + std::pow((second_y - first_y), 2));
                first_x = second_x;
                first_y = second_y;
            }
        }
        return length;
    }
    virtual CPoint const& operator[](size_t const& i) const { 
        try {
            return vertices_.at(i);
        }
        catch(const std::exception& e) {
            // std::cerr << "\n" << e.what() << "\nAn error occured! Returning (0,0) point\n";
            throw;
        }
        // return CPoint();
        // return *(std::unique_ptr<CPoint>(new CPoint()));
    }
protected:
    std::vector<CPoint> vertices_;
};

std::ostream& operator<<(std::ostream &os, CPolygonal_chain const &p);

class CClosed_polygonal_chain : public CPolygonal_chain {
public:
    CClosed_polygonal_chain(std::vector<CPoint> const& vertices = std::vector<CPoint>()) 
    : CPolygonal_chain(vertices) {
        if (vertices_.size() >= 3) {
            auto first_vertex = vertices_.begin();
            for (auto third_vertex = vertices_.end() - 1; third_vertex != vertices_.begin(); ) {
                auto second_vertex = third_vertex - 1;
                if (((third_vertex->x() - first_vertex->x()) * (second_vertex->y() - first_vertex->y())) \
                == ((third_vertex->y() - first_vertex->y()) * (second_vertex->x() - first_vertex->x()))) {
                    third_vertex = vertices_.erase(third_vertex);
                    --third_vertex;
                }
                else {
                    break;
                }
            }
        }
    }
    ~CClosed_polygonal_chain() = default;
    CClosed_polygonal_chain(CClosed_polygonal_chain const &other) 
    : CPolygonal_chain(other) {}
    CClosed_polygonal_chain(CClosed_polygonal_chain && other) : CPolygonal_chain(std::move(other)) {}
    CClosed_polygonal_chain &operator=(CClosed_polygonal_chain && other) {
        CPolygonal_chain::operator=(std::move(other));
        return *this;
    };
    CClosed_polygonal_chain &operator=(CClosed_polygonal_chain const& other) {
        CPolygonal_chain::operator=(other);
        return *this;
    }
    CPoint const& operator[](size_t const& i) const override {
        try {
            if (size()) {
                return vertices_.at(i % vertices_.size());
            }
            throw std::logic_error("Divide by zero");
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    double length() const override {
        double length = 0;
        if (size() > 1) {
            length = CPolygonal_chain::length() + \
            CVector(vertices_[vertices_.size() - 1], vertices_[0]).length();
        }
        return length;
    }
    bool has_self_intersections() const {
        //std::cout << size() << "\n";
        std::ofstream fout("log");
        if (size() > 3) {
            for (auto first_point = vertices_.begin(); first_point != vertices_.end() - 2; ++first_point) {
                auto second_point = first_point + 1;
                fout << "\n" << *first_point << " " << *second_point << " 2:";
                for (auto third_point = second_point + 1; third_point != vertices_.end(); ++third_point) {
                    auto fourth_point = (third_point == vertices_.end() - 1) ? vertices_.begin() : third_point + 1;
                    fout << *third_point << " " << *fourth_point << " ";
                    double a1 = second_point->y() - first_point->y();
                    fout << " A1 " << a1;
                    double b1 = first_point->x() - second_point->x();
                    fout << " B1 " << b1;
                    double c1 = first_point->y() * (-b1) - first_point->x() * a1;
                    fout << " C1 " << c1;
                    double a2 = fourth_point->y() - third_point->y();
                    fout << " A2 " << a2;
                    double b2 = third_point->x() - fourth_point->x();
                    fout << " B2 " << b2;
                    double c2 = third_point->y() * (-b2) - third_point->x() * a2;
                    fout << " C2 " << c2;
                    double zn = a1 * b2 - b1 * a2; 
                    fout << " ZN " << zn;
                    if (zn != 0) {
                        double x = static_cast<double>(-(c1 * b2 - b1 * c2) / zn);
                        double y = static_cast<double>(-(a1 * c2 - c1 * a2) / zn);
                        fout << " X " << x << " Y " << y << " ";
                        if ((first_point->x() < x && x < second_point->x() ||\
                        first_point->x() > x && x > second_point->x()) &&\
                        (third_point->x() < x && x > fourth_point->x() ||\
                        third_point->x() > x && x > fourth_point->x())\
                        && (first_point->y() < y && y < second_point->y() ||\
                        first_point->y() > y && y > second_point->y()) &&\
                        (third_point->y() < y && y < fourth_point->y() ||\
                        third_point->y() > y && y > fourth_point->y())) {
                            return true;
                        }
                    }
                    else {
                        double k = static_cast<double>(a1 / a2);
                        if (b1 == k * b2 && c1 == k * c2) {
                            return true;
                        }
                    }
                }
            
            }
        }
        return false;
    }
};

class IShape {
public:
    IShape() {}
    virtual ~IShape() {};
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual size_t size() const = 0;
    virtual std::vector<double> sides() const = 0;
    virtual std::vector<double> angles() const = 0;
    virtual bool is_convex() const = 0;
};


class CPolygon : public IShape {
public:
    CPolygon(std::vector<CPoint> const& vertices = std::vector<CPoint>()) 
    : edges_(vertices) {
        try {
            if (edges_.has_self_intersections()) {
                throw std::logic_error("Given polygon has self intersections");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CPolygon(std::initializer_list<CPoint> const& points) 
    : edges_(points) {
        try {
            if (edges_.has_self_intersections()) {
                throw std::logic_error("Given polygon has self intersections");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CPolygon(CClosed_polygonal_chain const& contour)
    : edges_(contour) {
        try {
            if (edges_.has_self_intersections()) {
                throw std::logic_error("Given polygon has self intersections");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    ~CPolygon() = default;
    CPolygon(CPolygon const& other) : edges_(other.edges_) {}
    CPolygon(CPolygon && other) : edges_(std::move(other.edges_)) {}
    CPolygon &operator=(CPolygon const& other) {
        edges_ = other.edges_;
        return *this;
    }
    CPolygon &operator=(CPolygon && other) {
        edges_ = std::move(other.edges_);
        return *this;
    }
    CPoint const& operator[](size_t const& i) const {
        return edges_[i];
    }
    void swap(CPolygon &other) { edges_.swap(other.edges_); }
    double perimeter() const override {
        return edges_.length();
    }
    size_t size() const override {
        return edges_.size();
    }
    virtual double area() const {
        double area = 0;
        if (size() > 2) {
            for (size_t i = 0; i < size(); ++i) {
                area += edges_[i].x() * edges_[i + 1].y() -\
                edges_[i + 1].x() * edges_[i].y();
            }
        }
        return std::abs(area) / 2;
    }
    bool is_convex() const {
        if (size() > 2) {
            CVector first_side(edges_[0], edges_[1]);
            CVector second_side(edges_[1], edges_[2]);
            double prev_val = cross_prod_val(first_side, second_side);
            for (size_t i = 2; i < size(); ++i) {
                first_side = second_side;
                second_side = {edges_[i], edges_[i + 1]};
                double curr_val = cross_prod_val(first_side, second_side);
                if (curr_val * prev_val < 0) {
                    return false;
                }
                prev_val = curr_val;
            }
        }
        return true;
    }
    virtual std::vector<double> sides() const {
        std::vector<double> sides(size());
        for (size_t i = 0; i < size(); ++i) {
            CVector side(edges_[i], edges_[i + 1]);
            sides[i] = side.length();
        }
        return sides;
    }
    virtual std::vector<double> angles() const {
        std::vector<double> angles(size());
        for (size_t i = 0; i < 3; ++i) {
            CVector first_side(edges_[i], edges_[i + 1]);
            CVector second_side(edges_[i], edges_[i + 2]);
            angles[i] = std::acos((first_side * second_side) / (first_side.length() * second_side.length()))\
            * 180 / M_PI;
        }
        return angles;
    }
    bool is_regular() const {
        if (size() > 2) {
            std::vector<double> sides_len = sides();
            std::vector<double> angles_val = angles();
            for (double const& side : sides_len) {
                //std::cout << side << "\n";
                if (side != sides_len[0]) {
                    return false;
                }
            }
            for (double const& angle : angles_val) {
                //std::cout << angle << "\n";
                if (angle != angles_val[0]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
protected:
    CClosed_polygonal_chain edges_;
};

std::ostream& operator<<(std::ostream &os, CPolygon const &p);

class CTriangle : public CPolygon {
public:
enum class Triangle_Types_Angle {
    RIGHT,
    OBTUSE,
    ACUTE
};
enum class Triangle_Types_Sides {
    EQUILATERAL,
    ISOSCELES,
    SCALENE
};
    CTriangle(std::vector<CPoint> const& vertices = {CPoint(0,0), CPoint(0,1), CPoint(1,0)}) 
    : CPolygon(vertices) {
        try {
            if (size() != 3) {
                throw std::logic_error("Invalid triangle!");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CTriangle(std::initializer_list<CPoint> const& vertices) : CPolygon(vertices) {
        try {
            if (size() != 3) {
                throw std::logic_error("Invalid triangle!");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CTriangle(CPoint const& a, CPoint const& b, CPoint const& c) : CPolygon({a, b, c}) {
        try {
            if (size() != 3) {
                throw std::logic_error("Invalid triangle!");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CTriangle(CTriangle const& other) : CPolygon(other) {}
    CTriangle(CTriangle && other) : CPolygon(std::move(other)) {}
    ~CTriangle() = default;
    CTriangle &operator=(CTriangle const &other) {
        CPolygon::operator=(other);
        return *this;
    }
    CTriangle &operator=(CTriangle && other) {
        CPolygon::operator=(std::move(other));
        return *this;
    }
    double area() const override {
        std::vector<double> sides_v = sides();
        double perim_half = perimeter() / 2;
        return std::sqrt(perim_half * (perim_half - sides_v[0]) * (perim_half - sides_v[1]) * (perim_half - sides_v[2]));
    }
    Triangle_Types_Angle angle_type() const {
        std::vector<double> angles_ = angles();
        for (auto angle : angles_) {
            if (angle == 90) {
                return Triangle_Types_Angle::RIGHT;
            }
            if (angle > 90) {
                return Triangle_Types_Angle::OBTUSE;
            }
        }
        return Triangle_Types_Angle::ACUTE;
    }
    Triangle_Types_Sides side_type() const {
        std::vector<double> sides_len = sides();
        if (sides_len[0] == sides_len[1] && sides_len[0] == sides_len[2]) {
            return Triangle_Types_Sides::EQUILATERAL;
        }
        if (sides_len[0] == sides_len[1] || sides_len[0] == sides_len[2] || sides_len[1] == sides_len[2]) {
            return Triangle_Types_Sides::ISOSCELES;
        }
        return Triangle_Types_Sides::SCALENE;
    }
    double median(size_t const& i) const {
        std::vector<double> sides_len = sides();
        return std::sqrt(2 * std::pow(sides_len[i % 3], 2) + \
        2 * std::pow(sides_len[(i + 1) % 3], 2) - std::pow(sides_len[i - 1], 2)) / 2;
    }
    double bisector(size_t const& i) const {
        double per_half = perimeter() / 2;
        std::vector<double> sides_len = sides();
        return (2 * std::sqrt(sides_len[i % 3] * sides_len[(i + 1) % 3] * per_half * (per_half - sides_len[i - 1])))\
        / (sides_len[i % 3] + sides_len[(i + 1) % 3]);
    }
    double height(size_t const& i) const {
        CVector side(edges_[i - 1], edges_[i % 3]);
        return (2 * area()) / side.length();
    }
    double incircle_radius() const {
        return (2 * area()) / perimeter();
    }
    double circumscribed_radius() const {
        std::vector<double> sides_len = sides();
        double result = 1;
        for (double const& side : sides_len) {
            result *= side;
        }
        result /= (4 * area());
        return result;
    }
};
inline std::ostream& operator<<(std::ostream &os, CTriangle::Triangle_Types_Angle const& type) { 
    switch(type) {
        case CTriangle::Triangle_Types_Angle::RIGHT:
            os << "right";
            break;
        case CTriangle::Triangle_Types_Angle::OBTUSE:
            os << "obtuse";
            break;
        case CTriangle::Triangle_Types_Angle::ACUTE:
            os << "acute";
            break;
    } 
    return os; 
}
inline std::ostream& operator<<(std::ostream &os, CTriangle::Triangle_Types_Sides const &type) { 
    switch(type) {
        case CTriangle::Triangle_Types_Sides::EQUILATERAL:
            os << "equilateral";
            break;
        case CTriangle::Triangle_Types_Sides::ISOSCELES:
            os << "isosceles";
            break;
        case CTriangle::Triangle_Types_Sides::SCALENE:
            os << "scalene";
            break;
    } 
    return os; 
}

class CTrapezoid : public CPolygon {
public:
    CTrapezoid(std::vector<CPoint> const& vertices = {CPoint(-2,0), CPoint(-1,1), CPoint(1,1), CPoint(2,0)})
    : CPolygon(vertices) {
        try {
            if (!is_trapezoid()) {
                throw std::logic_error("Invalid trapezoid!");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CTrapezoid(CPoint const& a, CPoint const& b, CPoint const& c, CPoint const& d) 
    : CPolygon({a, b, c, d}) {
        try {
            if (!is_trapezoid()) {
                throw std::logic_error("Invalid trapezoid!");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CTrapezoid(CTrapezoid const& other) : CPolygon(other) {}
    CTrapezoid(CTrapezoid && other) : CPolygon(std::move(other)) {}
    ~CTrapezoid() = default;
    CTrapezoid &operator=(CTrapezoid const& other) {
        CPolygon::operator=(other);
        return *this;
    } 
    CTrapezoid &operator=(CTrapezoid && other) {
        CPolygon::operator=(std::move(other));
        return *this;
    }
    double area() const override {
        std::pair<double, double> bases_v = bases();
        std::pair<double, double> legs_v = legs();
        double a = std::min(bases_v.first, bases_v.second);
        double b = std::max(bases_v.first, bases_v.second);
        return (a + b) / (4 * (b - a)) * std::sqrt((a + legs_v.first + legs_v.second - b) * (a + legs_v.second - b - legs_v.first) *\
        (a + legs_v.first - b - legs_v.second) * (b + legs_v.first + legs_v.second - a));
    }

    bool is_isosceles() const {
        std::pair<double, double> legs_v = legs();
        return (legs_v.first == legs_v.second);
    }
    bool is_right() const {
        std::vector<double> angles_val = angles();
        return std::any_of(angles_val.begin(), angles_val.end(), [](double const& angle) { return angle == 90; });
    }
    double midsegment() const {
        std::pair<double, double> bases_v = bases();
        return (bases_v.first + bases_v.second) / 2;
    }
    double height() const {
        return area() / midsegment();
    }
    double circumscribed_radius() const {
        if (is_isosceles()) {
            std::pair<double, double> bases_v = bases();
            std::pair<double, double> legs_v = legs();
            return std::sqrt((bases_v.first * bases_v.second + legs_v.first * legs_v.second)\
             / (4 - std::pow((bases_v.first - bases_v.second) / legs_v.first, 2)));
        }
        else {
            return 0;
        }
    }
    double inscribed_radius() const {
        std::pair<double, double> bases_v = bases();
        std::pair<double, double> legs_v = legs();
        if (bases_v.first + bases_v.second == legs_v.first + legs_v.second) {
            return height() / 2;
        }
        else {
            return 0;
        }
    }
private:
    std::pair<double, double> bases() const {
        CVector first_side(edges_[0], edges_[1]);
        CVector third_side(edges_[2], edges_[3]);
        if (first_side.y() * third_side.x() == first_side.x() * third_side.y()) {
            return std::make_pair(first_side.length(), third_side.length());
        }
        else {
            return std::make_pair(CVector(edges_[1], edges_[2]).length(), CVector(edges_[3], edges_[0]).length());
        }
    }
    std::pair<double, double> legs() const {
        CVector first_side(edges_[0], edges_[1]);
        CVector third_side(edges_[2], edges_[3]);
        if (first_side.y() * third_side.x() != first_side.x() * third_side.y()) {
            return std::make_pair(first_side.length(), third_side.length());
        }
        else {
            return std::make_pair(CVector(edges_[1], edges_[2]).length(), CVector(edges_[3], edges_[0]).length());
        }
    }
    bool is_trapezoid() const {
        CVector first_side(edges_[0], edges_[1]);
        CVector second_side(edges_[1], edges_[2]);
        CVector third_side(edges_[2], edges_[3]);
        CVector fourth_side(edges_[3], edges_[0]);
        if (first_side.y() * third_side.x() == first_side.x() * third_side.y()) {
            return second_side.y() * fourth_side.x() != second_side.x() * fourth_side.y();
        }
        return second_side.y() * fourth_side.x() == second_side.x() * fourth_side.y();
    }
};


class CRegular_polygon : public CPolygon {
public:
    CRegular_polygon(std::vector<CPoint> const& vertices = {CPoint(-1,-1), CPoint(-1,1), CPoint(1,1), CPoint(1,-1)})
    : CPolygon(vertices) {
        try {
            if (!(static_cast<CPolygon>(*this).is_regular())) {
                throw std::logic_error("Given polygon is irregular");
            }
        }
        catch (const std::exception& e) {
            //std::cerr << e.what() << '\n';
            throw;
        }
    }
    CRegular_polygon(size_t const& n, size_t const& radius, \
    double const& angle, CPoint const& center = {0,0}) {
        if (n >= 3) {
            std::vector<CPoint> edges;
            for (size_t i = 0; i < n; ++i) {
                double radian_angle = (angle + (360 * i) / n) * (M_PI / 180);
                double x_i = center.x() + radius * std::cos(radian_angle);
                double y_i = center.y() + radius * std::sin(radian_angle);
                edges.push_back(CPoint(x_i, y_i));
            }
            edges_ = edges;
        }
        else {
            try {
                throw std::logic_error("Unable to construct such a polygon");
            }
            catch (const std::exception& e) {
                throw;
            }
        }
    }
    CRegular_polygon(CRegular_polygon const& other) : CPolygon(other) {}
    CRegular_polygon(CRegular_polygon && other) : CPolygon(std::move(other)) {}
    ~CRegular_polygon() = default;
    CRegular_polygon &operator=(CRegular_polygon const& other) {
        CPolygon::operator=(other);
        return *this;
    } 
    CRegular_polygon &operator=(CRegular_polygon && other) {
        CPolygon::operator=(std::move(other));
        return *this;
    }
    double side() const {
        return CVector(edges_[0], edges_[1]).length();
    }
    std::vector<double> sides() const override {
        return std::vector<double>(size(), side());
    }
    double perimeter() const override {
        return side() * size();
    }
    double area() const override {
        return (perimeter() * side()) / (4 * std::tan(M_PI / size()));
    }
    double angle() const {
        return (static_cast<double>((size() - 2)) / size()) * 180;
    }
    std::vector<double> angles() const override {
        return std::vector<double>(size(), angle());
    }
    double incircle_radius() const {
        return side() / (2 * std::tan(M_PI / size()));
    }
    double circumscribed_radius() const {
        return side() / (2 * std::sin(M_PI / size()));
    }
};

#endif