#include "figures.hpp"

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
