#ifndef FIGURES_HPP
#define FIGURES_HPP
#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include <memory>

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

class Vector : public ILine {
public:
    Vector(CPoint const& start, CPoint const& end) : start_(start), end_(end) {}
    ~Vector() = default;
    Vector(Vector const& other) : start_(other.start_), end_(other.end_) {}
    Vector &operator=(Vector const& other) {
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
    double operator*(Vector const& other) {
        return x() * other.x() + y() * other.y();
    }
    friend double cross_prod_val(Vector const& a, Vector const& b) {
        //first_side.x() * second_side.y() - second_side.x() * first_side.y()
        return a.x() * b.y() - b.x() * a.y();
    }
private:
    CPoint start_;
    CPoint end_;
};


class Polygonal_chain : public ILine {
public:
    Polygonal_chain(std::vector<CPoint> const& vertices = std::vector<CPoint>()) 
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
    size_t size() const { return vertices_.size(); }
    Polygonal_chain(Polygonal_chain const& other) 
    : vertices_(other.vertices_) {}
    Polygonal_chain &operator=(Polygonal_chain const& other) {
        if (this != &other) {
            vertices_ = other.vertices_;
        }
        return *this;
    }
    Polygonal_chain(Polygonal_chain && other) {
        swap(other);
    }
    Polygonal_chain &operator=(Polygonal_chain && other) {
        swap(other);
        return *this;
    }
    void swap(Polygonal_chain &other) { std::swap(vertices_, other.vertices_); }
    ~Polygonal_chain() = default;
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
    virtual CPoint operator[](size_t i) const { 
        try {
            return vertices_.at(i);
        }
        catch(const std::exception& e) {
            std::cerr << "\n" << e.what() << "\nAn error occured! Returning (0,0) point\n";
        }
        return CPoint();
        // return *(std::unique_ptr<CPoint>(new CPoint()));
    }
protected:
    std::vector<CPoint> vertices_;
};

class Closed_polygonal_chain : public Polygonal_chain {
public:
    Closed_polygonal_chain(std::vector<CPoint> const& vertices = std::vector<CPoint>()) 
    : Polygonal_chain(vertices) {
        if (vertices_.size() >= 3) {
            auto first_vertex = vertices_.begin();
            for (auto third_vertex = vertices_.end() - 1; third_vertex != vertices_.begin(); ) {
                auto second_vertex = third_vertex - 1;
                if (((third_vertex->x() - first_vertex->x()) * (second_vertex->y() - first_vertex->y())) \
                == ((third_vertex->y() - first_vertex->y()) * (second_vertex->x() - first_vertex->x()))) {
                    third_vertex = vertices_.erase(third_vertex);
                }
                else {
                    break;
                }
            }
        }
    }
    ~Closed_polygonal_chain() = default;
    Closed_polygonal_chain(Closed_polygonal_chain const &other) 
    : Polygonal_chain(other) {}
    Closed_polygonal_chain(Closed_polygonal_chain && other) : Polygonal_chain(other) {}
    Closed_polygonal_chain &operator=(Closed_polygonal_chain && other) {
        Polygonal_chain::operator=(other);
        return *this;
    };
    Closed_polygonal_chain &operator=(Closed_polygonal_chain const& other) {
        if (this != &other) {
            Polygonal_chain::operator=(other);
        }
        return *this;
    }
    CPoint operator[](size_t i) const override {
        try {
            return vertices_.at(i % vertices_.size());
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
        return CPoint();
    }
    double length() const override {
        double length = 0;
        if (size() > 1) {
            length = Polygonal_chain::length() + Vector(vertices_[vertices_.size() - 1], vertices_[0]).length();
            //std::sqrt(std::pow(vertices_[0].x() - vertices_[vertices_.size() - 1].x(), 2)\
            + std::pow(vertices_[0].y() - vertices_[vertices_.size() - 1].y(), 2));
        }
        return length;
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


class Polygon : public IShape {
public:
    Polygon(std::vector<CPoint> const& vertices = std::vector<CPoint>()) 
    : edges_(vertices) {}
    Polygon(std::initializer_list<CPoint> const& points) 
    : edges_(points) {}
    Polygon(Closed_polygonal_chain const& contour)
    : edges_(contour) {}
    ~Polygon() = default;
    Polygon(Polygon const& other) : edges_(other.edges_) {}
    Polygon(Polygon && other) : edges_(other.edges_) {}
    Polygon &operator=(Polygon const& other) {
        edges_ = other.edges_;
        return *this;
    }
    Polygon &operator=(Polygon && other) {
        edges_ = other.edges_;
        return *this;
    }
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
            Vector first_side(edges_[0], edges_[1]);
            Vector second_side(edges_[1], edges_[2]);
            //CPoint first_side(edges_[1].x() - edges_[0].x(), edges_[1].y() - edges_[0].y());
            //CPoint second_side(edges_[2].x() - edges_[1].x(), edges_[2].y() - edges_[1].y());
            //double prev_val = first_side.x() * second_side.y() - second_side.x() * first_side.y(); 
            double prev_val = cross_prod_val(first_side, second_side);
            for (size_t i = 2; i < size(); ++i) {
                first_side = second_side;
                second_side = {edges_[i], edges_[i + 1]};
                //second_side = {edges_[i + 1].x() - edges_[i].x(), edges_[i + 1].y() - edges_[i].y()};
                //double curr_val = first_side.x() * second_side.y() - second_side.x() * first_side.y();
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
            Vector side(edges_[i], edges_[i + 1]);
            sides[i] = side.length();
        }
        return sides;
    }
    virtual std::vector<double> angles() const {
        std::vector<double> angles(size());
        for (size_t i = 0; i < 3; ++i) {
            //CPoint first_side(edges_[(i + 1) % 3].x() - edges_[i].x(),\
            edges_[(i + 1) % 3].y() - edges_[i].y());
            //CPoint second_side(edges_[(i + 2) % 3].x() - edges_[i].x(),\
            edges_[(i + 2) % 3].y() - edges_[i].y());
            Vector first_side(edges_[i], edges_[i + 1]);
            Vector second_side(edges_[i], edges_[i + 2]);
            //angles[i] = std::acos((first_side.x() * second_side.x() + first_side.y() * second_side.y()) /\
            (std::sqrt(std::pow(first_side.x(), 2) + std::pow(first_side.y(), 2)) *\
            std::sqrt(std::pow(second_side.x(), 2) + std::pow(second_side.y(), 2)))) * 180 / std::acos(-1.);
            angles[i] = std::acos((first_side * second_side) / (first_side.length() * second_side.length()))\
            * 180 / std::acos(-1.);
        }
        return angles;
    }
protected:
    Closed_polygonal_chain edges_;
};

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
inline std::ostream& operator<<(std::ostream &os, Triangle_Types_Angle const& type) { 
    switch(type) {
        case Triangle_Types_Angle::RIGHT:
            std::cout << "right";
            break;
        case Triangle_Types_Angle::OBTUSE:
            std::cout << "obtuse";
            break;
        case Triangle_Types_Angle::ACUTE:
            std::cout << "acute";
            break;
    } 
    return os; 
}
inline std::ostream& operator<<(std::ostream &os, Triangle_Types_Sides const &type) { 
    switch(type) {
        case Triangle_Types_Sides::EQUILATERAL:
            std::cout << "equilateral";
            break;
        case Triangle_Types_Sides::ISOSCELES:
            std::cout << "isosceles";
            break;
        case Triangle_Types_Sides::SCALENE:
            std::cout << "scalene";
            break;
    } 
    return os; 
}
class Triangle : public Polygon {
public:
    Triangle(std::vector<CPoint> const& vertices = {CPoint(0,0), CPoint(0,1), CPoint(1,0)}) 
    : Polygon(vertices) {
        if (size() != 3) {
            //
        }
    }
    Triangle(std::initializer_list<CPoint> const& vertices) : Polygon(vertices) {
        if (size() != 3) {
            //
        }
    }
    Triangle(CPoint const& a, CPoint const& b, CPoint const& c) : Polygon({a, b, c}) {
        if (size() != 3) {
            //
        }
    }
    Triangle(Triangle const& other) : Polygon(other) {}
    Triangle(Triangle && other) : Polygon(other) {}
    ~Triangle() = default;
    Triangle &operator=(Triangle const &other) {
        Polygon::operator=(other);
        return *this;
    }
    Triangle &operator=(Triangle && other) {
        Polygon::operator=(other);
        return *this;
    }
    // std::vector<double> angles() const {
    //     std::vector<double> angles(3);
    //     for (size_t i = 0; i < 3; ++i) {
    //         //CPoint first_side(edges_[(i + 1) % 3].x() - edges_[i].x(),\
    //         edges_[(i + 1) % 3].y() - edges_[i].y());
    //         //CPoint second_side(edges_[(i + 2) % 3].x() - edges_[i].x(),\
    //         edges_[(i + 2) % 3].y() - edges_[i].y());
    //         Vector first_side(edges_[i], edges_[i + 1]);
    //         Vector second_side(edges_[i], edges_[i + 2]);
    //         //angles[i] = std::acos((first_side.x() * second_side.x() + first_side.y() * second_side.y()) /\
    //         (std::sqrt(std::pow(first_side.x(), 2) + std::pow(first_side.y(), 2)) *\
    //         std::sqrt(std::pow(second_side.x(), 2) + std::pow(second_side.y(), 2)))) * 180 / std::acos(-1.);
    //         angles[i] = std::acos((first_side * second_side) / (first_side.length() * second_side.length()))\
    //         * 180 / std::acos(-1.);
    //     }
    //     return angles;
    // }

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
        // Vector a(edges_[0], edges_[1]);
        // Vector b(edges_[1], edges_[2]);
        // Vector c(edges_[2], edges_[0]);
        // double len_a = a.length();
        // double len_b = b.length();
        // double len_c = c.length();
        // if (len_a == len_b && len_b == len_c) { return EQUILATERAL; }
        // if (len_a == len_b || len_a == len_c || len_b == len_c) { return ISOSCELES; }
        // return SCALENE;
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
        Vector side(edges_[i - 1], edges_[i % 3]);
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


class Trapezoid : public Polygon {
public:
    Trapezoid(std::vector<CPoint> const& vertices = {CPoint(-2,0), CPoint(-1,1), CPoint(1,1), CPoint(2,0)})
    : Polygon(vertices) {}
    Trapezoid(Trapezoid const& other) : Polygon(other) {}
    Trapezoid(Trapezoid && other) : Polygon(other) {}
    ~Trapezoid() = default;
    Trapezoid &operator=(Trapezoid const& other) {
        Polygon::operator=(other);
        return *this;
    } 
    Trapezoid &operator=(Trapezoid && other) {
        Polygon::operator=(other);
        return *this;
    }
};


class Regular_polygon : public Polygon {
public:
    Regular_polygon(std::vector<CPoint> const& vertices = {CPoint(-1,-1), CPoint(-1,1), CPoint(1,1), CPoint(1,-1)})
    : Polygon(vertices) {}
    Regular_polygon(Regular_polygon const& other) : Polygon(other) {}
    Regular_polygon(Regular_polygon && other) : Polygon(other) {}
    ~Regular_polygon() = default;
    Regular_polygon &operator=(Regular_polygon const& other) {
        Polygon::operator=(other);
        return *this;
    } 
    Regular_polygon &operator=(Regular_polygon && other) {
        Polygon::operator=(other);
        return *this;
    }
    double side() const {
        //return (std::sqrt(std::pow(edges_[1].x() - edges_[0].x(), 2) +\
        std::pow(edges_[1].y() - edges_[0].y(), 2)));
        return Vector(edges_[0], edges_[1]).length();
    }
    std::vector<double> sides() const override {
        return std::vector<double>(size(), side());
    }
    double perimeter() const override {
        return side() * size();
    }
    double area() const override {
        return (perimeter() * side()) / (4 * std::tan(std::acos(-1.) / size()));
    }
    double angle() const {
        return ((size() - 2) / size()) * 180;
    }
    std::vector<double> angles() const override {
        return std::vector<double>(size(), angle());
    }
    double incircle_radius() const {
        return side() / (2 * std::tan(std::acos(-1.) / size()));
    }
    double circumscribed_radius() const {
        return side() / (2 * std::sin(std::acos(-1.) / size()));
    }
};


#endif