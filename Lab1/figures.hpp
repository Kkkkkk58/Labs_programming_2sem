#ifndef FIGURES_HPP
#define FIGURES_HPP
#include <vector>
#include <iostream>

// Класс "точка"
class CPoint {
public:
    CPoint(double const& = 0, double const& = 0);
    CPoint(CPoint const&);
    CPoint(CPoint &&);
    CPoint &operator=(CPoint const&);
    CPoint &operator=(CPoint &&);
    CPoint operator-() const;
    CPoint &operator+=(CPoint const&);
    CPoint &operator-=(CPoint const&);
    CPoint &operator++();
    CPoint operator++(int);
    CPoint &operator--();
    CPoint operator--(int);
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

const CPoint operator+(CPoint, CPoint const&);
const CPoint operator-(CPoint, CPoint const&);
std::ostream& operator<<(std::ostream &, CPoint const&);
bool on_the_same_line(CPoint const&, CPoint const&, CPoint const&);


// Интерфейс "линейных" объектов (имеющих длину, но не имеющих площади)
class ILine {
public:
    ILine() {}
    virtual double length() const = 0;
    virtual ~ILine() {};
};


// Класс "вектор" (напрямую в лабораторной не требуется, используется как вспомогательный)
class CVector : public ILine {
public:
    explicit CVector(CPoint const& = {0, 0}, CPoint const& = {0, 1});
    ~CVector() = default;
    CVector(CVector const&);
    CVector(CVector &&);
    CVector &operator=(CVector const&);
    CVector &operator=(CVector &&);
    double x() const;
    double y() const;
    double length() const;
    const double operator*(CVector const&);
    bool includes_point(CPoint const&) const;
private:
    void swap(CVector &);
    CPoint start_;
    CPoint end_;
};

double cross_prod_val(CVector const&, CVector const&);


// Класс "ломаная", отвечающий интерфейсу "линейных" объектов
class CPolygonal_chain : public ILine {
public:
    explicit CPolygonal_chain(std::vector<CPoint> const& = std::vector<CPoint>());
    CPolygonal_chain(CPolygonal_chain const&);
    CPolygonal_chain &operator=(CPolygonal_chain const&);
    CPolygonal_chain(CPolygonal_chain &&);
    CPolygonal_chain &operator=(CPolygonal_chain &&);
    virtual ~CPolygonal_chain() = default;
    virtual size_t size() const;
    virtual double length() const override;
    virtual CPoint const& operator[](size_t) const;
private:
    void correct_line();
protected:
    std::vector<CPoint> vertices_;
    void swap(CPolygonal_chain &);
};


std::ostream& operator<<(std::ostream &, CPolygonal_chain const&);


// Класс "замкнутая ломаная", унаследованный от класса "ломаная"
class CClosed_polygonal_chain : public CPolygonal_chain {
public:
    explicit CClosed_polygonal_chain(std::vector<CPoint> const& = std::vector<CPoint>());
    ~CClosed_polygonal_chain() = default;
    CClosed_polygonal_chain(CClosed_polygonal_chain const&);
    CClosed_polygonal_chain(CClosed_polygonal_chain &&);
    CClosed_polygonal_chain &operator=(CClosed_polygonal_chain &&);
    CClosed_polygonal_chain &operator=(CClosed_polygonal_chain const&);
    CPoint const& operator[](size_t) const override;
    double length() const override;
    bool has_self_intersections() const;
    void swap(CClosed_polygonal_chain &);
private:
    void correct_closed_line();
};


// Интерфейс для "плоских" объектов (имеющих периметр и площадь)
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
    virtual bool is_regular() const = 0;
};


// Класс "многоугольник", отвечающий интерфейсу "плоских" объектов
class CPolygon : public IShape {
public:
    explicit CPolygon(std::vector<CPoint> const& = std::vector<CPoint>());
    explicit CPolygon(std::initializer_list<CPoint> const&);
    CPolygon(CClosed_polygonal_chain const&);
    virtual ~CPolygon() = default;
    CPolygon(CPolygon const&);
    CPolygon(CPolygon &&);
    CPolygon &operator=(CPolygon const&);
    CPolygon &operator=(CPolygon &&);
    CPoint const& operator[](size_t const&) const;
    double perimeter() const override;
    size_t size() const override;
    virtual double area() const;
    virtual bool is_convex() const;
    virtual std::vector<double> sides() const;
    virtual std::vector<double> angles() const;
    virtual bool is_regular() const;
    friend std::ostream& operator<<(std::ostream &, CPolygon const&);
protected:
    CClosed_polygonal_chain vertices_;
private:
    void swap(CPolygon &);
};



// Класс "треугольник", унаследованный от "многоугольника"
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
    explicit CTriangle(std::vector<CPoint> const& = {CPoint(0,0), CPoint(0,1), CPoint(1,0)});
    explicit CTriangle(std::initializer_list<CPoint> const&);
    CTriangle(CPoint const&, CPoint const&, CPoint const&);
    CTriangle(CTriangle const&);
    CTriangle(CTriangle &&);
    ~CTriangle() = default;
    CTriangle &operator=(CTriangle const&);
    CTriangle &operator=(CTriangle &&);
    double area() const override;
    Triangle_types_angle angle_type() const;
    Triangle_types_sides side_type() const;
    double median(size_t) const;
    double bisector(size_t) const;
    double height(size_t) const;
    double inscribed_radius() const;
    double circumscribed_radius() const;
    bool is_convex() const override;
};

std::ostream& operator<<(std::ostream &, CTriangle::Triangle_types_angle const&);
std::ostream& operator<<(std::ostream &, CTriangle::Triangle_types_sides const&);


// Класс "трапеция", унаследованный от "многоугольника"
class CTrapezoid : public CPolygon {
public:
    explicit CTrapezoid(std::vector<CPoint> const& = {CPoint(-2,0), CPoint(-1,1), CPoint(1,1), CPoint(2,0)});
    explicit CTrapezoid(std::initializer_list<CPoint> const&);
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
    bool is_convex() const override;
private:
    std::pair<double, double> bases() const;
    std::pair<double, double> legs() const;
    bool is_trapezoid() const;
};


// Класс "правильный многоугольник", унаследованный от "многоугольника"
class CRegular_polygon : public CPolygon {
public:
    explicit CRegular_polygon(std::vector<CPoint> const& = {CPoint(-1,-1), CPoint(-1,1), CPoint(1,1), CPoint(1,-1)});
    explicit CRegular_polygon(std::initializer_list<CPoint> const&);
    CRegular_polygon(size_t const&, size_t const&, double const&, CPoint const& = {0,0});
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
    double inscribed_radius() const;
    double circumscribed_radius() const;
    bool is_convex() const override;
    bool is_regular() const override;
};

#endif