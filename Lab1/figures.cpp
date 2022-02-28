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

CVector::CVector(CPoint const &start, CPoint const &end) : start_(start), end_(end) {}
CVector::CVector(CVector const &other) : start_(other.start_), end_(other.end_) {}
CVector & CVector::operator=(CVector const &other) {
    if (this != &other) {
        start_ = other.start_;
        end_ = other.end_;
    }
    return *this;
}
double CVector::x() const {
    return end_.x() - start_.x();
}
double CVector::y() const {
    return end_.y() - start_.y();
}
double CVector::length() const {
    return std::sqrt(x() * x() + y() * y());
}
double CVector::operator*(CVector const &other) {
    return x() * other.x() + y() * other.y();
}
double cross_prod_val(CVector const &a, CVector const &b) {
    // first_side.x() * second_side.y() - second_side.x() * first_side.y()
    return a.x() * b.y() - b.x() * a.y();
}

CPolygonal_chain::CPolygonal_chain(std::vector<CPoint> const &vertices)
    : vertices_(std::vector<CPoint>(vertices)) {
    correct_line();
}
size_t CPolygonal_chain::size() const { return vertices_.size(); }
CPolygonal_chain::CPolygonal_chain(CPolygonal_chain const &other)
    : vertices_(other.vertices_) {}
CPolygonal_chain & CPolygonal_chain::operator=(CPolygonal_chain const &other) {
    if (this != &other) {
        vertices_ = other.vertices_;
    }
    return *this;
}
CPolygonal_chain::CPolygonal_chain(CPolygonal_chain &&other) {
    swap(other);
}
CPolygonal_chain & CPolygonal_chain::operator=(CPolygonal_chain &&other) {
    swap(other);
    return *this;
}
void CPolygonal_chain::swap(CPolygonal_chain &other) { std::swap(vertices_, other.vertices_); }
double CPolygonal_chain::length() const {
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
CPoint const & CPolygonal_chain::operator[](size_t const &i) const {
    try {
        return vertices_.at(i);
    }
    catch (const std::exception &e) {
        throw;
    }
}

void CPolygonal_chain::correct_line() {
    if (vertices_.size() > 1) {
        for (auto adj_finder = std::adjacent_find(vertices_.begin(), vertices_.end());
             adj_finder != vertices_.end();) {
            adj_finder = vertices_.erase(adj_finder);
            adj_finder = std::adjacent_find(adj_finder, vertices_.end());
        }
    }
    if (vertices_.size() >= 3) {
        for (auto third_vertex = vertices_.begin() + 2; third_vertex != vertices_.end();)
        {
            auto first_vertex = third_vertex - 2;
            auto second_vertex = third_vertex - 1;
            if (((third_vertex->x() - first_vertex->x()) * (second_vertex->y() - first_vertex->y())) == ((third_vertex->y() - first_vertex->y()) * (second_vertex->x() - first_vertex->x()))) {
                third_vertex = vertices_.erase(third_vertex);
            }
            else {
                ++third_vertex;
            }
        }
    }
}

std::ostream& operator<<(std::ostream &os, CPolygonal_chain const &p) { 
    for (size_t i = 0; i < p.size(); ++i) {
        os << p[i] << " ";
    }
    return os; 
}

CClosed_polygonal_chain::CClosed_polygonal_chain(std::vector<CPoint> const &vertices)
    : CPolygonal_chain(vertices) {
    correct_closed_line();
}
CClosed_polygonal_chain::CClosed_polygonal_chain(CClosed_polygonal_chain const &other)
    : CPolygonal_chain(other) {}
CClosed_polygonal_chain::CClosed_polygonal_chain(CClosed_polygonal_chain &&other) : CPolygonal_chain(std::move(other)) {}
CClosed_polygonal_chain & CClosed_polygonal_chain::operator=(CClosed_polygonal_chain &&other) {
    CPolygonal_chain::operator=(std::move(other));
    return *this;
};
CClosed_polygonal_chain & CClosed_polygonal_chain::operator=(CClosed_polygonal_chain const &other) {
    CPolygonal_chain::operator=(other);
    return *this;
}
CPoint const & CClosed_polygonal_chain::operator[](size_t const &i) const {
    try {
        if (size()) {
            return vertices_.at(i % vertices_.size());
        }
        throw std::logic_error("Divide by zero");
    }
    catch (const std::exception &e) {
        throw;
    }
}
double CClosed_polygonal_chain::length() const {
    double length = 0;
    if (size() > 1) {
        length = CPolygonal_chain::length() +
                 CVector(vertices_[vertices_.size() - 1], vertices_[0]).length();
    }
    return length;
}
bool CClosed_polygonal_chain::has_self_intersections() const
{
    if (size() > 3) {
        for (auto first_point = vertices_.begin(); first_point != vertices_.end() - 2; ++first_point) {
            auto second_point = first_point + 1;
            for (auto third_point = second_point + 1; third_point != vertices_.end(); ++third_point) {
                auto fourth_point = (third_point == vertices_.end() - 1) ? vertices_.begin() : third_point + 1;
                double a1 = second_point->y() - first_point->y();
                double b1 = first_point->x() - second_point->x();
                double c1 = first_point->y() * (-b1) - first_point->x() * a1;
                double a2 = fourth_point->y() - third_point->y();
                double b2 = third_point->x() - fourth_point->x();
                double c2 = third_point->y() * (-b2) - third_point->x() * a2;
                double zn = a1 * b2 - b1 * a2;
                if (zn != 0) {
                    double x = static_cast<double>(-(c1 * b2 - b1 * c2) / zn);
                    double y = static_cast<double>(-(a1 * c2 - c1 * a2) / zn);
                    if ((first_point->x() < x && x < second_point->x() ||
                         first_point->x() > x && x > second_point->x()) &&
                        (third_point->x() < x && x > fourth_point->x() ||
                         third_point->x() > x && x > fourth_point->x()) &&
                        (first_point->y() < y && y < second_point->y() ||
                         first_point->y() > y && y > second_point->y()) &&
                        (third_point->y() < y && y < fourth_point->y() ||
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

void CClosed_polygonal_chain::correct_closed_line() {
    if (vertices_.size() >= 3) {
        auto first_vertex = vertices_.begin();
        for (auto third_vertex = vertices_.end() - 1; third_vertex != vertices_.begin();) {
            auto second_vertex = third_vertex - 1;
            if (((third_vertex->x() - first_vertex->x()) * (second_vertex->y() - first_vertex->y())) == ((third_vertex->y() - first_vertex->y()) * (second_vertex->x() - first_vertex->x()))) {
                third_vertex = vertices_.erase(third_vertex);
                --third_vertex;
            }
            else {
                break;
            }
        }
    }
}

CPolygon::CPolygon(std::vector<CPoint> const &vertices)
    : edges_(vertices) {
    try {
        if (edges_.has_self_intersections()) {
            throw std::logic_error("Given polygon has self intersections");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CPolygon::CPolygon(std::initializer_list<CPoint> const &points)
    : edges_(points) {
    try {
        if (edges_.has_self_intersections()) {
            throw std::logic_error("Given polygon has self intersections");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CPolygon::CPolygon(CClosed_polygonal_chain const &contour)
    : edges_(contour) {
    try {
        if (edges_.has_self_intersections()) {
            throw std::logic_error("Given polygon has self intersections");
        }
    }
    catch (const std::exception &e) {
        // std::cerr << e.what() << '\n';
        throw;
    }
}
CPolygon::CPolygon(CPolygon const &other) : edges_(other.edges_) {}
CPolygon::CPolygon(CPolygon &&other) : edges_(std::move(other.edges_)) {}
CPolygon & CPolygon::operator=(CPolygon const &other) {
    edges_ = other.edges_;
    return *this;
}
CPolygon & CPolygon::operator=(CPolygon &&other) {
    edges_ = std::move(other.edges_);
    return *this;
}
CPoint const & CPolygon::operator[](size_t const &i) const {
    return edges_[i];
}
void CPolygon::CPolygon::swap(CPolygon &other) { edges_.swap(other.edges_); }
double CPolygon::perimeter() const {
    return edges_.length();
}
size_t CPolygon::size() const {
    return edges_.size();
}
double CPolygon::area() const {
    double area = 0;
    if (size() > 2) {
        for (size_t i = 0; i < size(); ++i) {
            area += edges_[i].x() * edges_[i + 1].y() -
                    edges_[i + 1].x() * edges_[i].y();
        }
    }
    return std::abs(area) / 2;
}
bool CPolygon::is_convex() const {
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
std::vector<double> CPolygon::sides() const {
    std::vector<double> sides(size());
    for (size_t i = 0; i < size(); ++i) {
        CVector side(edges_[i], edges_[i + 1]);
        sides[i] = side.length();
    }
    return sides;
}
std::vector<double> CPolygon::angles() const {
    std::vector<double> angles(size());
    for (size_t i = 0; i < size(); ++i) {
        CVector first_side(edges_[i], edges_[i + 1]);
        CVector second_side(edges_[i], edges_[i + 2]);
        angles[i] = std::acos((first_side * second_side) / (first_side.length() * second_side.length())) * 180 / std::acos(-1.L);
    }
    return angles;
}
bool CPolygon::is_regular() const {
    if (size() > 2) {
        std::vector<double> sides_len = sides();
        std::vector<double> angles_val = angles();
        for (double const &side : sides_len) {
            if (side != sides_len[0]) {
                return false;
            }
        }
        for (double const &angle : angles_val) {
            if (angle != angles_val[0]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

std::ostream& operator<<(std::ostream &os, CPolygon const &p) { 
    std::string polygon_type = typeid(p).name();
    polygon_type.erase(0,1);
    os << polygon_type << " ";
    for (size_t i = 0; i < p.size(); ++i) {
        os << p[i] << " ";
    }
    return os; 
}

CTriangle::CTriangle(std::vector<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        if (size() != 3) {
            throw std::logic_error("Invalid triangle!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CTriangle::CTriangle(std::initializer_list<CPoint> const &vertices) : CPolygon(vertices) {
    try {
        if (size() != 3) {
            throw std::logic_error("Invalid triangle!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CTriangle::CTriangle(CPoint const &a, CPoint const &b, CPoint const &c) : CPolygon({a, b, c}) {
    try {
        if (size() != 3) {
            throw std::logic_error("Invalid triangle!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CTriangle::CTriangle(CTriangle const &other) : CPolygon(other) {}
CTriangle::CTriangle(CTriangle &&other) : CPolygon(std::move(other)) {}
CTriangle & CTriangle::operator=(CTriangle const &other) {
    CPolygon::operator=(other);
    return *this;
}
CTriangle & CTriangle::operator=(CTriangle &&other) {
    CPolygon::operator=(std::move(other));
    return *this;
}
double CTriangle::area() const {
    std::vector<double> sides_v = sides();
    double perim_half = perimeter() / 2;
    return std::sqrt(perim_half * (perim_half - sides_v[0]) * (perim_half - sides_v[1]) * (perim_half - sides_v[2]));
}
CTriangle::Triangle_types_angle CTriangle::angle_type() const {
    std::vector<double> angles_ = angles();
    for (auto angle : angles_) {
        if (angle == 90) {
            return Triangle_types_angle::RIGHT;
        }
        if (angle > 90) {
            return Triangle_types_angle::OBTUSE;
        }
    }
    return Triangle_types_angle::ACUTE;
}
CTriangle::Triangle_types_sides CTriangle::side_type() const {
    std::vector<double> sides_len = sides();
    if (sides_len[0] == sides_len[1] && sides_len[0] == sides_len[2]) {
        return Triangle_types_sides::EQUILATERAL;
    }
    if (sides_len[0] == sides_len[1] || sides_len[0] == sides_len[2] || sides_len[1] == sides_len[2]) {
        return Triangle_types_sides::ISOSCELES;
    }
    return Triangle_types_sides::SCALENE;
}
double CTriangle::median(size_t const &i) const {
    std::vector<double> sides_len = sides();
    return std::sqrt(2 * std::pow(sides_len[i % 3], 2) +
                     2 * std::pow(sides_len[(i + 1) % 3], 2) - std::pow(sides_len[i - 1], 2)) / 2;
}
double CTriangle::bisector(size_t const &i) const {
    double per_half = perimeter() / 2;
    std::vector<double> sides_len = sides();
    return (2 * std::sqrt(sides_len[i % 3] * sides_len[(i + 1) % 3] * per_half * (per_half - sides_len[i - 1]))) / (sides_len[i % 3] + sides_len[(i + 1) % 3]);
}
double CTriangle::height(size_t const &i) const {
    CVector side(edges_[i - 1], edges_[i % 3]);
    return (2 * area()) / side.length();
}
double CTriangle::incircle_radius() const {
    return (2 * area()) / perimeter();
}
double CTriangle::circumscribed_radius() const {
    std::vector<double> sides_len = sides();
    double result = 1;
    for (double const &side : sides_len) {
        result *= side;
    }
    result /= (4 * area());
    return result;
}

std::ostream& operator<<(std::ostream &os, CTriangle::Triangle_types_angle const& type) { 
    switch(type) {
        case CTriangle::Triangle_types_angle::RIGHT:
            os << "right";
            break;
        case CTriangle::Triangle_types_angle::OBTUSE:
            os << "obtuse";
            break;
        case CTriangle::Triangle_types_angle::ACUTE:
            os << "acute";
            break;
    } 
    return os; 
}

std::ostream& operator<<(std::ostream &os, CTriangle::Triangle_types_sides const &type) { 
    switch(type) {
        case CTriangle::Triangle_types_sides::EQUILATERAL:
            os << "equilateral";
            break;
        case CTriangle::Triangle_types_sides::ISOSCELES:
            os << "isosceles";
            break;
        case CTriangle::Triangle_types_sides::SCALENE:
            os << "scalene";
            break;
    } 
    return os; 
}

CTrapezoid::CTrapezoid(std::vector<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        if (!is_trapezoid()) {
            throw std::logic_error("Invalid trapezoid!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CTrapezoid::CTrapezoid(CPoint const &a, CPoint const &b, CPoint const &c, CPoint const &d)
    : CPolygon({a, b, c, d}) {
    try {
        if (!is_trapezoid()) {
            throw std::logic_error("Invalid trapezoid!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CTrapezoid::CTrapezoid(CTrapezoid const &other) : CPolygon(other) {}
CTrapezoid::CTrapezoid(CTrapezoid &&other) : CPolygon(std::move(other)) {}
CTrapezoid & CTrapezoid::operator=(CTrapezoid const &other) {
    CPolygon::operator=(other);
    return *this;
}
CTrapezoid & CTrapezoid::operator=(CTrapezoid &&other) {
    CPolygon::operator=(std::move(other));
    return *this;
}
double CTrapezoid::area() const {
    std::pair<double, double> bases_v = bases();
    std::pair<double, double> legs_v = legs();
    double a = std::min(bases_v.first, bases_v.second);
    double b = std::max(bases_v.first, bases_v.second);
    return (a + b) / (4 * (b - a)) * std::sqrt((a + legs_v.first + legs_v.second - b) * (a + legs_v.second - b - legs_v.first) * (a + legs_v.first - b - legs_v.second) * (b + legs_v.first + legs_v.second - a));
}

bool CTrapezoid::is_isosceles() const {
    std::pair<double, double> legs_v = legs();
    return (legs_v.first == legs_v.second);
}
bool CTrapezoid::is_right() const {
    std::vector<double> angles_val = angles();
    return std::any_of(angles_val.begin(), angles_val.end(), [](double const &angle)
                       { return angle == 90; });
}
double CTrapezoid::midsegment() const {
    std::pair<double, double> bases_v = bases();
    return (bases_v.first + bases_v.second) / 2;
}
double CTrapezoid::height() const {
    return area() / midsegment();
}
double CTrapezoid::circumscribed_radius() const {
    if (is_isosceles()) {
        std::pair<double, double> bases_v = bases();
        std::pair<double, double> legs_v = legs();
        return std::sqrt((bases_v.first * bases_v.second + legs_v.first * legs_v.second) / (4 - std::pow((bases_v.first - bases_v.second) / legs_v.first, 2)));
    }
    else {
        return 0;
    }
}
double CTrapezoid::inscribed_radius() const {
    std::pair<double, double> bases_v = bases();
    std::pair<double, double> legs_v = legs();
    if (bases_v.first + bases_v.second == legs_v.first + legs_v.second) {
        return height() / 2;
    }
    else {
        return 0;
    }
}

std::pair<double, double> CTrapezoid::bases() const {
    CVector first_side(edges_[0], edges_[1]);
    CVector third_side(edges_[2], edges_[3]);
    if (first_side.y() * third_side.x() == first_side.x() * third_side.y()) {
        return std::make_pair(first_side.length(), third_side.length());
    }
    else {
        return std::make_pair(CVector(edges_[1], edges_[2]).length(), CVector(edges_[3], edges_[0]).length());
    }
}
std::pair<double, double> CTrapezoid::legs() const {
    CVector first_side(edges_[0], edges_[1]);
    CVector third_side(edges_[2], edges_[3]);
    if (first_side.y() * third_side.x() != first_side.x() * third_side.y()) {
        return std::make_pair(first_side.length(), third_side.length());
    }
    else {
        return std::make_pair(CVector(edges_[1], edges_[2]).length(), CVector(edges_[3], edges_[0]).length());
    }
}
bool CTrapezoid::is_trapezoid() const {
    CVector first_side(edges_[0], edges_[1]);
    CVector second_side(edges_[1], edges_[2]);
    CVector third_side(edges_[2], edges_[3]);
    CVector fourth_side(edges_[3], edges_[0]);
    if (first_side.y() * third_side.x() == first_side.x() * third_side.y()) {
        return second_side.y() * fourth_side.x() != second_side.x() * fourth_side.y();
    }
    return second_side.y() * fourth_side.x() == second_side.x() * fourth_side.y();
}

CRegular_polygon::CRegular_polygon(std::vector<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        if (!((static_cast<CPolygon>(*this)).is_regular())) {
            throw std::logic_error("Given polygon is irregular");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
CRegular_polygon::CRegular_polygon(size_t const &n, size_t const &radius,
                 double const &angle, CPoint const &center) {
    if (n >= 3) {
        std::vector<CPoint> edges;
        for (size_t i = 0; i < n; ++i) {
            double radian_angle = (angle + (360 * i) / n) * (std::acos(-1.L) / 180);
            double x_i = center.x() + radius * std::cos(radian_angle);
            double y_i = center.y() + radius * std::sin(radian_angle);
            edges.push_back(CPoint(x_i, y_i));
        }
        edges_ = edges;
    }
    else {
        try {
            throw std::logic_error("Unable to construct such polygon");
        }
        catch (const std::exception &e) {
            throw;
        }
    }
}
CRegular_polygon::CRegular_polygon(CRegular_polygon const &other) : CPolygon(other) {}
CRegular_polygon::CRegular_polygon(CRegular_polygon &&other) : CPolygon(std::move(other)) {}
CRegular_polygon & CRegular_polygon::operator=(CRegular_polygon const &other) {
    CPolygon::operator=(other);
    return *this;
}
CRegular_polygon & CRegular_polygon::operator=(CRegular_polygon &&other) {
    CPolygon::operator=(std::move(other));
    return *this;
}
double CRegular_polygon::side() const {
    return CVector(edges_[0], edges_[1]).length();
}
std::vector<double> CRegular_polygon::sides() const {
    return std::vector<double>(size(), side());
}
double CRegular_polygon::perimeter() const {
    return side() * size();
}
double CRegular_polygon::area() const {
    return (perimeter() * side()) / (4 * std::tan(std::acos(-1.L) / size()));
}
double CRegular_polygon::angle() const {
    return (static_cast<double>((size() - 2)) / size()) * 180;
}
std::vector<double> CRegular_polygon::angles() const {
    return std::vector<double>(size(), angle());
}
double CRegular_polygon::incircle_radius() const {
    return side() / (2 * std::tan(std::acos(-1.L) / size()));
}
double CRegular_polygon::circumscribed_radius() const {
    return side() / (2 * std::sin(std::acos(-1.L) / size()));
}