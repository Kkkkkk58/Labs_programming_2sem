#include "figures.hpp"
#include <string>
CPoint::CPoint(double const& x, double const& y) : x_(x), y_(y) {}
CPoint::CPoint(CPoint const& other) : x_(other.x_), y_(other.y_) {}
CPoint::CPoint(CPoint && other) { swap(other); }
CPoint & CPoint::operator=(CPoint const& other) {
    if (this != &other) {
        x_ = other.x_;
        y_ = other.y_;
    }
    return *this;
}
CPoint & CPoint::operator=(CPoint && other) {
    swap(other);
    return *this;
}
CPoint CPoint::operator+(CPoint const& other) const { return CPoint(x_ + other.x_, y_ + other.y_); }
CPoint CPoint::operator-() const { return CPoint(-x_, -y_); }
CPoint CPoint::operator-(CPoint const& other) const { return CPoint(x_ - other.x_, y_ - other.y_); }
CPoint & CPoint::operator+=(CPoint const &other) {
    x_ += other.x_;
    y_ += other.y_;
    return *this;
}
CPoint & CPoint::operator-=(CPoint const &other) {
    x_ -= other.x_;
    y_ -= other.y_;
    return *this;
}
CPoint & CPoint::operator++() {
    ++x_;
    ++y_;
    return *this;
}
CPoint CPoint::operator++(int) {
    CPoint tmp(*this);
    ++(*this);
    return tmp;
}
double CPoint::operator[](size_t i) const { return (i == 0 ? x_ : y_); }
bool CPoint::operator==(CPoint const& other) const { return x_ == other.x_ && y_ == other.y_; }
bool CPoint::operator!=(CPoint const& other) const { return !(*this == other); }
void CPoint::swap(CPoint &other) {
    std::swap(x_, other.x_);
    std::swap(y_, other.y_);
}

double CPoint::x() const { return x_; }
double CPoint::y() const { return y_; }


std::ostream& operator<<(std::ostream &os, CPoint const &p) { 
    os << '(' << p.x() << ", " << p.y() << ')'; 
    return os; 
}

std::ostream& operator<<(std::ostream &os, Polygonal_chain const &p) { 
    for (size_t i = 0; i < p.size(); ++i) {
        os << p[i] << " ";
    }
    return os; 
}

std::ostream& operator<<(std::ostream &os, Polygon const &p) { 
    std::string polygon_type = typeid(p).name();
    polygon_type.erase(0,1);
    os << polygon_type << " ";
    for (size_t i = 0; i < p.size(); ++i) {
        os << p[i] << " ";
    }
    return os; 
}

