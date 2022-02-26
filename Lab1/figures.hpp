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
    CPoint(double const& = 0, double const& = 0);
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
    CVector(CPoint const&, CPoint const&);
    ~CVector() = default;
    CVector(CVector const&);
    CVector &operator=(CVector const&);
    double x() const;
    double y() const;
    double length() const;
    double operator*(CVector const&);
    friend double cross_prod_val(CVector const&, CVector const&);
private:
    CPoint start_;
    CPoint end_;
};


class CPolygonal_chain : public ILine {
public:
    CPolygonal_chain(std::vector<CPoint> const& = std::vector<CPoint>());
    virtual size_t size() const;
    CPolygonal_chain(CPolygonal_chain const&);
    CPolygonal_chain &operator=(CPolygonal_chain const&);
    CPolygonal_chain(CPolygonal_chain &&);
    CPolygonal_chain &operator=(CPolygonal_chain &&);
    void swap(CPolygonal_chain &);
    ~CPolygonal_chain() = default;
    virtual double length() const override;
    virtual CPoint const& operator[](size_t const&) const;
private:
    void correct_line();
protected:
    std::vector<CPoint> vertices_;
};

std::ostream& operator<<(std::ostream &, CPolygonal_chain const &);

class CClosed_polygonal_chain : public CPolygonal_chain {
public:
    CClosed_polygonal_chain(std::vector<CPoint> const& = std::vector<CPoint>());
    ~CClosed_polygonal_chain() = default;
    CClosed_polygonal_chain(CClosed_polygonal_chain const &other);
    CClosed_polygonal_chain(CClosed_polygonal_chain && other);
    CClosed_polygonal_chain &operator=(CClosed_polygonal_chain && other);
    CClosed_polygonal_chain &operator=(CClosed_polygonal_chain const& other);
    CPoint const& operator[](size_t const& i) const override;
    double length() const override;
    bool has_self_intersections() const;
private:
    void correct_closed_line();
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
    CPolygon(std::vector<CPoint> const& = std::vector<CPoint>());
    CPolygon(std::initializer_list<CPoint> const&);
    CPolygon(CClosed_polygonal_chain const&);
    ~CPolygon() = default;
    CPolygon(CPolygon const&);
    CPolygon(CPolygon &&);
    CPolygon &operator=(CPolygon const&);
    CPolygon &operator=(CPolygon &&);
    CPoint const& operator[](size_t const&) const;
    void swap(CPolygon &);
    double perimeter() const override;
    size_t size() const override;
    virtual double area() const;
    bool is_convex() const;
    virtual std::vector<double> sides() const;
    virtual std::vector<double> angles() const;
    bool is_regular() const;
protected:
    CClosed_polygonal_chain edges_;
};

std::ostream& operator<<(std::ostream &os, CPolygon const &p);

class CTriangle : public CPolygon {
public:
    enum class Triangle_types_angle {
        RIGHT,
        OBTUSE,
        ACUTE
    };
    enum class Triangle_types_sides {
        EQUILATERAL,
        ISOSCELES,
        SCALENE
    };
    CTriangle(std::vector<CPoint> const& = {CPoint(0,0), CPoint(0,1), CPoint(1,0)});
    CTriangle(std::initializer_list<CPoint> const&);
    CTriangle(CPoint const&, CPoint const&, CPoint const&);
    CTriangle(CTriangle const&);
    CTriangle(CTriangle &&);
    ~CTriangle() = default;
    CTriangle &operator=(CTriangle const &);
    CTriangle &operator=(CTriangle &&);
    double area() const override;
    Triangle_types_angle angle_type() const;
    Triangle_types_sides side_type() const;
    double median(size_t const&) const;
    double bisector(size_t const&) const;
    double height(size_t const&) const;
    double incircle_radius() const;
    double circumscribed_radius() const;
};

std::ostream& operator<<(std::ostream &os, CTriangle::Triangle_types_angle const& type);
std::ostream& operator<<(std::ostream &os, CTriangle::Triangle_types_sides const &type);

class CTrapezoid : public CPolygon {
public:
    CTrapezoid(std::vector<CPoint> const& = {CPoint(-2,0), CPoint(-1,1), CPoint(1,1), CPoint(2,0)});
    CTrapezoid(CPoint const&, CPoint const&, CPoint const&, CPoint const&);
    CTrapezoid(CTrapezoid const&);
    CTrapezoid(CTrapezoid &&);
    ~CTrapezoid() = default;
    CTrapezoid &operator=(CTrapezoid const&);
    CTrapezoid &operator=(CTrapezoid &&);
    double area() const override;
    bool is_isosceles() const;
    bool is_right() const;
    double midsegment() const;
    double height() const;
    double circumscribed_radius() const;
    double inscribed_radius() const;
private:
    std::pair<double, double> bases() const;
    std::pair<double, double> legs() const;
    bool is_trapezoid() const;
};


class CRegular_polygon : public CPolygon {
public:
    CRegular_polygon(std::vector<CPoint> const& = {CPoint(-1,-1), CPoint(-1,1), CPoint(1,1), CPoint(1,-1)});
    CRegular_polygon(size_t const&, size_t const&, \
    double const&, CPoint const& = {0,0});
    CRegular_polygon(CRegular_polygon const&);
    CRegular_polygon(CRegular_polygon &&);
    ~CRegular_polygon() = default;
    CRegular_polygon &operator=(CRegular_polygon const&);
    CRegular_polygon &operator=(CRegular_polygon &&);
    double side() const;
    std::vector<double> sides() const override;
    double perimeter() const override;
    double area() const override;
    double angle() const;
    std::vector<double> angles() const override;
    double incircle_radius() const;
    double circumscribed_radius() const;
};

#endif