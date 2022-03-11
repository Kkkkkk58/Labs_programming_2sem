#include "figures.hpp"
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <memory>
#include <typeinfo>
#include <utility>
#include <fstream>

constexpr double EPS = 1e-9;        // Константа времени исполнения "эпсилон" для выполнения сравнения чисел с плавающей точкой
const double PI = std::acos(-1.L);  // Математическая константа - число пи
const double TO_DEGREES = 180 / PI; // Постоянный множитель для перевода угла из радиан в градусы
const double TO_RADIANS = PI / 180; // Постоянный множитель для перевода угла из градусов в радианы

// Конструктор класса СPoint (в т.ч. по умолчанию) от двух координат точки
CPoint::CPoint(double const &x, double const &y) : x_(x), y_(y) {}
// Конструктор копирования класса CPoint
CPoint::CPoint(CPoint const &other) : x_(other.x_), y_(other.y_) {}
// Конструктор перемещения класса CPoint
CPoint::CPoint(CPoint && other) { swap(other); }
// Оператор присваивания класса CPoint
CPoint & CPoint::operator=(CPoint const &other) {
    if (this != &other) {
        x_ = other.x_;
        y_ = other.y_;
    }
    return *this;
}
// Перемещающий оператор присваивания класса CPoint
CPoint & CPoint::operator=(CPoint && other) {
    swap(other);
    return *this;
}
// Унарный оператор "-" класса CPoint
CPoint CPoint::operator-() const { return CPoint(-x_, -y_); }
// Оператор "+=" класса CPoint
CPoint & CPoint::operator+=(CPoint const &other) {
    x_ += other.x_;
    y_ += other.y_;
    return *this;
}
// Бинарный оператор сложения класса CPoint
const CPoint operator+(CPoint lhs, CPoint const &rhs) {
     return lhs += rhs; 
}
// Оператор "-=" класса CPoint
CPoint & CPoint::operator-=(CPoint const &other) {
    x_ -= other.x_;
    y_ -= other.y_;
    return *this;
}
// Бинарный оператор вычитания класса CPoint
const CPoint operator-(CPoint lhs, CPoint const &rhs) {
     return lhs -= rhs;
}
// Префиксный инкремент класса CPoint
CPoint & CPoint::operator++() {
    ++x_;
    ++y_;
    return *this;
}
// Постфиксный инкремент класса CPoint
CPoint CPoint::operator++(int) {
    CPoint tmp(*this);
    ++(*this);
    return tmp;
}
// Префиксный декремент класса CPoint
CPoint & CPoint::operator--() {
    --x_;
    --y_;
    return *this;
}
// Постфиксный декремент класса CPoint
CPoint CPoint::operator--(int) {
    CPoint tmp(*this);
    --(*this);
    return tmp;
}
// Оператор проверки на равенство класса CPoint
bool CPoint::operator==(CPoint const &other) const { 
    return std::abs(x_ - other.x_) <= EPS && std::abs(y_ - other.y_) <= EPS; 
}
// Оператор проверки на неравенство класса CPoint
bool CPoint::operator!=(CPoint const &other) const { return !(*this == other); }
// Метод swap класса CPoint - обмен координатами с другой точкой
void CPoint::swap(CPoint &other) {
    std::swap(x_, other.x_);
    std::swap(y_, other.y_);
}
// Метод, возвращающий координату точки по оси Ox
double CPoint::x() const { return x_; }
// Метод, возвращающий координату точки по оси Oy
double CPoint::y() const { return y_; }
// Функция проверки, принадлежат ли три точки одной прямой
bool on_the_same_line(CPoint const &a, CPoint const &b, CPoint const &c) {
    // Если принадлежат, то их координаты удовлетворяют каноническому уравнению прямой
    return std::abs(((c.x() - a.x()) * (b.y() - a.y())) - \
    ((c.y() - a.y()) * (b.x() - a.x()))) <= EPS;
}

// Перегрузка оператора вывода в поток объекта класса CPoint
std::ostream& operator<<(std::ostream &os, CPoint const &p) { 
    os << '(' << p.x() << ", " << p.y() << ')'; 
    return os; 
}


// Конструктор класса CVector (в т.ч. по умолчанию) от двух точек - начала и конца вектора
CVector::CVector(CPoint const &start, CPoint const &end) : start_(start), end_(end) {}
// Конструктор копирования класса CVector
CVector::CVector(CVector const &other) : start_(other.start_), end_(other.end_) {}
// Конструктор перемещения класса CVector
CVector::CVector(CVector && other) { swap(other); }
// Оператор присваивания класса CVector
CVector & CVector::operator=(CVector const &other) {
    if (this != &other) {
        start_ = other.start_;
        end_ = other.end_;
    }
    return *this;
}
// Перемещающий оператор присваивания класса CVector
CVector & CVector::operator=(CVector && other) {
    swap(other);
    return *this;
}
// Метод swap класса CVector - обмен началами и концами векторов
void CVector::swap(CVector &other) {
    start_.swap(other.start_);
    end_.swap(other.end_);
}
// Метод для получения координаты вектора по оси Ox
double CVector::x() const {
    return end_.x() - start_.x();
}
// Метод для получения координаты вектора по оси Oy
double CVector::y() const {
    return end_.y() - start_.y();
}
// Метод для получения длины вектора
double CVector::length() const {
    return std::sqrt(x() * x() + y() * y());
}
// Скалярное умножение векторов
const double CVector::operator*(CVector const &other) {
    return x() * other.x() + y() * other.y();
}
// Векторное произведение векторов
double cross_prod_val(CVector const &a, CVector const &b) {
    return a.x() * b.y() - b.x() * a.y();
}
// Проверка, включает ли вектор (сегмент) заданную точку
bool CVector::includes_point(CPoint const &p) const {
    return ((start_.x() < p.x() && p.x() < end_.x() || start_.x() > p.x() && p.x() > end_.x()) && 
             (start_.y() < p.y() && p.y() < end_.y() || start_.y() > p.y() && p.y() > end_.y()));
}

// Конструктор класса CPolygonal_chain от вектора точек
CPolygonal_chain::CPolygonal_chain(std::vector<CPoint> const &vertices)
    : vertices_(std::vector<CPoint>(vertices)) {
    correct_line();   // запуск корректировки ломаной (исключения вершин, лежащих на одной прямой)
}
// Конструктор копирования класса CPolygonal_chain
CPolygonal_chain::CPolygonal_chain(CPolygonal_chain const &other)
    : vertices_(other.vertices_) {}
// Перемещающий конструктор класса CPolygonal_chain
CPolygonal_chain::CPolygonal_chain(CPolygonal_chain &&other) {
    swap(other);
}
// Оператор присваивания класса CPolygonal_chain
CPolygonal_chain & CPolygonal_chain::operator=(CPolygonal_chain const &other) {
    if (this != &other) {
        vertices_ = other.vertices_;
    }
    return *this;
}
// Перемещающий оператор присваивания класса CPolygonal_chain
CPolygonal_chain & CPolygonal_chain::operator=(CPolygonal_chain &&other) {
    swap(other);
    return *this;
}
// Метод получения количества вершин ломаной
size_t CPolygonal_chain::size() const { return vertices_.size(); }
// Метод swap класса CPolygonal_chain - обмен вершинами
void CPolygonal_chain::swap(CPolygonal_chain &other) { std::swap(vertices_, other.vertices_); }
// Метод получения длины ломаной
double CPolygonal_chain::length() const {
    double length = 0;
    // Последовательно рассматриваются звенья ломаной, длины звеньев суммируются
    if (size() > 1) {
        auto iter = vertices_.begin();
        double first_x = iter->x();
        double first_y = iter->y();
        while (++iter != vertices_.end()) {
            length += CVector({first_x, first_y}, *iter).length();
            first_x = iter->x();
            first_y = iter->y();
        }
    }
    return length;
}
// Оператор [] для получения точки по индексу. Завершает программу при попытке доступа к несуществующей вершине
CPoint const & CPolygonal_chain::operator[](size_t i) const {
    try {
        return vertices_.at(i);
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Метод корректировки ломаной - исключения вершин, лежащих на одной прямой
void CPolygonal_chain::correct_line() {
    // Удаляем пары совпадающих вершин
    if (vertices_.size() > 1) {
        for (auto adj_finder = std::adjacent_find(vertices_.begin(), vertices_.end());
             adj_finder != vertices_.end();) {
            adj_finder = vertices_.erase(adj_finder);
            adj_finder = std::adjacent_find(adj_finder, vertices_.end());
        }
    }
    // Рассматриваем тройки вершин, если находим лежащие на одной прямой, удаляем третью из ломаной
    if (vertices_.size() >= 3) {
        for (auto third_vertex = vertices_.begin() + 2; third_vertex != vertices_.end(); ++third_vertex) {
            auto first_vertex = third_vertex - 2;
            auto second_vertex = third_vertex - 1;
            if (on_the_same_line(*first_vertex, *second_vertex, *third_vertex)) {
                third_vertex = vertices_.erase(second_vertex);
            }
        }
    }
}

// Перегрузка оператора вывода ломаной в поток
std::ostream& operator<<(std::ostream &os, CPolygonal_chain const &p) { 
    for (size_t i = 0; i < p.size(); ++i) {
        os << p[i] << " ";
    }
    return os; 
}


// Конструктор класса CClosed_polygonal_chain от вектора точек
CClosed_polygonal_chain::CClosed_polygonal_chain(std::vector<CPoint> const &vertices)
    : CPolygonal_chain(vertices) {
    correct_closed_line();  // запуск корректировки замкнутой ломаной (удаление вершин, лежащих на одной прямой с места замыкания)
}
// Конструктор копирования класса CClosed_polygonal_chain
CClosed_polygonal_chain::CClosed_polygonal_chain(CClosed_polygonal_chain const &other)
    : CPolygonal_chain(other) {}
// Конструктор перемещения класса CClosed_polygonal_chain
CClosed_polygonal_chain::CClosed_polygonal_chain(CClosed_polygonal_chain &&other) : CPolygonal_chain(std::move(other)) {}
// Оператор присваивания класса CClosed_polygonal_chain
CClosed_polygonal_chain & CClosed_polygonal_chain::operator=(CClosed_polygonal_chain const &other) {
    CPolygonal_chain::operator=(other);
    return *this;
}
// Перемещающий оператор присваивания класса CClosed_polygonal_chain
CClosed_polygonal_chain & CClosed_polygonal_chain::operator=(CClosed_polygonal_chain &&other) {
    CPolygonal_chain::operator=(std::move(other));
    return *this;
};
// Метод swap класса CClosed_polygonal_chain
void CClosed_polygonal_chain::swap(CClosed_polygonal_chain &other) {
    CPolygonal_chain::swap(other);
}
// Оператор [] для получения точки замкнутой ломаной - завершает программу в случае попытки нахождения точки в пустой ломаной
CPoint const & CClosed_polygonal_chain::operator[](size_t i) const {
    try {
        if (size()) {
            return vertices_.at(i % vertices_.size());
        }
        throw std::logic_error("Closed chain is empty");
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Метод получения длины замкнутой ломаной
double CClosed_polygonal_chain::length() const {
    double length = 0;
    // Длина равна длине обычной ломаной + длине последнего замыкающего звена
    if (size() > 1) {
        length = CPolygonal_chain::length() +\
                 CVector(vertices_[vertices_.size() - 1], vertices_[0]).length();
    }
    return length;
}
// Проверка на наличие самопересечений в замкнутой ломаной
bool CClosed_polygonal_chain::has_self_intersections() const {
    if (size() > 3) {
        // Проверяем каждые два звена ломаной на пересечения с помощью нахождения пересечения двух прямых
        for (auto first_point = vertices_.begin(); first_point != vertices_.end() - 2; ++first_point) {
            auto second_point = first_point + 1;
            CVector first_segment(*first_point, *second_point);
            for (auto third_point = second_point + 1; third_point != vertices_.end(); ++third_point) {
                auto fourth_point = (third_point == vertices_.end() - 1) ? vertices_.begin() : third_point + 1;
                CVector second_segment(*third_point, *fourth_point);
                double a1 = first_segment.y();
                double b1 = -first_segment.x();
                double c1 = first_point->y() * (-b1) - first_point->x() * a1;
                double a2 = second_segment.y();
                double b2 = -second_segment.x();
                double c2 = third_point->y() * (-b2) - third_point->x() * a2;
                double determinant = a1 * b2 - b1 * a2;
                // Находим точку пересечения прямых с помощью решения СЛАУ с двумя неизвестными методом Крамера
                if (determinant != 0) {
                    double intersection_x = static_cast<double>(-(c1 * b2 - b1 * c2) / determinant);
                    double intersection_y = static_cast<double>(-(a1 * c2 - c1 * a2) / determinant);
                    CPoint intersection_point(intersection_x, intersection_y);
                    // Проверяем, попала ли полученная точка в оба звена
                    if (first_segment.includes_point(intersection_point) && second_segment.includes_point(intersection_point)) {
                        return true;
                    }
                }
                // Если знаменатель матрицы равен нулю, то прямые либо параллельны, либо совпадают
                else {
                    double k = static_cast<double>(a1 / a2);
                    // Если прямые совпадают (их коэффициенты отличаются в k раз) - ломаная имеет самопересечения
                    if (std::abs(b1 - k * b2) <= EPS && std::abs(c1 - k * c2) <= EPS) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
// Метод корректировки замкнутой ломаной - удаление вершин, лежащих на одной прямой в обратном порядке, начиная с замыкающей точки
void CClosed_polygonal_chain::correct_closed_line() {
    if (vertices_.size() >= 4) {
        auto first_vertex = vertices_.begin();
        auto second_vertex = first_vertex + 1;
        // Рассматриваем четверку вершин - берем две из начала и две из конца
        for (auto fourth_vertex = vertices_.end() - 1; fourth_vertex != second_vertex + 1;) {
            auto third_vertex = fourth_vertex - 1;
            // Если две вершины из начала и конечная лежат на одной прямой - удаляем начальную
            if (on_the_same_line(*first_vertex, *second_vertex, *fourth_vertex)) {
                first_vertex = vertices_.erase(first_vertex);
                ++second_vertex;
            }
            // Если две вершины из конца и начальная лежат на одной прямой - удаляем конечную
            else if (on_the_same_line(*third_vertex, *fourth_vertex, *first_vertex)) {
                fourth_vertex = vertices_.erase(fourth_vertex);
                --fourth_vertex;
                --third_vertex;
            }
            // Если для четверки вышеперечисленное не выполняется - заканчиваем проверку
            else {
                break;
            }
        }
    }
}


// Конструктор класса CPolygon от вектора точек
CPolygon::CPolygon(std::vector<CPoint> const &vertices)
    : vertices_(vertices) {
    try {
        // Если предложенный набор вершин образует самопересекающуюся ломаную - ошибка
        if (vertices_.has_self_intersections()) {
            throw std::logic_error("Given polygon has self intersections");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CPolygon от initializer_list точек
CPolygon::CPolygon(std::initializer_list<CPoint> const &points)
    : vertices_(points) {
    try {
        // Если предложенный набор вершин образует самопересекающуюся ломаную - ошибка
        if (vertices_.has_self_intersections()) {
            throw std::logic_error("Given polygon has self intersections");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CPolygon от контура многоугольника (замкнутой ломаной)
CPolygon::CPolygon(CClosed_polygonal_chain const &contour)
    : vertices_(contour) {
    try {
        // Если предложенный набор вершин образует самопересекающуюся ломаную - ошибка
        if (vertices_.has_self_intersections()) {
            throw std::logic_error("Given polygon has self intersections");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор копирования класса CPolygon
CPolygon::CPolygon(CPolygon const &other) : vertices_(other.vertices_) {}
// Конструктор перемещения класса CPolygon
CPolygon::CPolygon(CPolygon &&other) : vertices_(std::move(other.vertices_)) {}
// Оператор присваивания класса CPolygon
CPolygon & CPolygon::operator=(CPolygon const &other) {
    vertices_ = other.vertices_;
    return *this;
}
// Перемещающий оператор присваивания класса CPolygon
CPolygon & CPolygon::operator=(CPolygon &&other) {
    vertices_ = std::move(other.vertices_);
    return *this;
}
// Оператор [] для получения вершины многоугольника по индексу
CPoint const & CPolygon::operator[](size_t const &i) const {
    return vertices_[i];
}
// Метод swap для обмена вершинами многоугольников
void CPolygon::swap(CPolygon &other) { vertices_.swap(other.vertices_); }
// Метод получения периметра многоугольника
double CPolygon::perimeter() const {
    return vertices_.length();
}
// Метод получения количества вершин многоугольника
size_t CPolygon::size() const {
    return vertices_.size();
}
// Метод получения плозади многоугольника
double CPolygon::area() const {
    double area = 0;
    // Алгоритм подсчёта площади использует формулу площади Гаусса (формулу шнуровки)
    if (size() > 2) {
        for (size_t i = 0; i < size(); ++i) {
            area += vertices_[i].x() * vertices_[i + 1].y() -
                    vertices_[i + 1].x() * vertices_[i].y();
        }
    }
    return std::abs(area) / 2;
}
// Проверка выпуклости многоугольника
bool CPolygon::is_convex() const {
    if (size() > 2) {
        CVector first_side(vertices_[0], vertices_[1]);
        CVector second_side(vertices_[1], vertices_[2]);
        double prev_val = cross_prod_val(first_side, second_side);
        for (size_t i = 2; i < size(); ++i) {
            first_side = second_side;
            second_side = CVector(vertices_[i], vertices_[i + 1]);
            double curr_val = cross_prod_val(first_side, second_side);
            // Если многоугольник выпуклый, то последовательно взятые векторные произведения сторон имеют один знак
            if (curr_val * prev_val < -EPS) {
                return false;
            }
            prev_val = curr_val;
        }
    }
    return true;
}
// Метод для получения вектора с длинами сторон многоугольника
std::vector<double> CPolygon::sides() const {
    std::vector<double> sides(size());
    for (size_t i = 0; i < size(); ++i) {
        CVector side(vertices_[i], vertices_[i + 1]);
        sides[i] = side.length();
    }
    return sides;
}
// Метод для получения вектора со значениями углов многоугольника в градусах
std::vector<double> CPolygon::angles() const {
    std::vector<double> angles(size());
    for (size_t i = 0; i < size(); ++i) {
        CVector first_side(vertices_[i], vertices_[i + 1]);
        CVector second_side(vertices_[i], vertices_[i + 2]);
        // Используется скалярное произведение векторов сторон
        angles[i] = std::acos((first_side * second_side) / (first_side.length() * second_side.length())) * TO_DEGREES;
    }
    return angles;
}
// Проверка на правильность многоугольника
bool CPolygon::is_regular() const {
    if (size() > 2) {
        std::vector<double> sides_len = sides();
        std::vector<double> angles_val = angles();
        // Проверка равенства сторон
        for (double const &side : sides_len) {
            if (std::abs(side - sides_len[0]) > EPS) {
                return false;
            }
        }
        // Проверка равенства углов
        for (double const &angle : angles_val) {
            if (std::abs(angle - angles_val[0]) > EPS) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// Перегрузка оператора вывода для многоугольника
std::ostream& operator<<(std::ostream &os, CPolygon const &p) { 
    // Получение типа многоугольника
    std::string polygon_type = typeid(p).name();
    polygon_type.erase(0,2);
    os << polygon_type << " ";
    // Печать ограничивающей ломаной
    os << p.vertices_;
    return os; 
}


// Конструктор класса CTriangle от вектора точек
CTriangle::CTriangle(std::vector<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        // Если в треугольнике не три вершины - ошибка
        if (size() != 3) {
            throw std::logic_error("Invalid triangle!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CTriangle от initializer_list точек
CTriangle::CTriangle(std::initializer_list<CPoint> const &vertices) : CPolygon(vertices) {
    try {
        // Если в треугольнике не три вершины - ошибка
        if (size() != 3) {
            throw std::logic_error("Invalid triangle!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CTriangle от трёх точек
CTriangle::CTriangle(CPoint const &a, CPoint const &b, CPoint const &c) : CPolygon({a, b, c}) {
    try {
        // Если в треугольнике не три вершины - ошибка
        if (size() != 3) {
            throw std::logic_error("Invalid triangle!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор копирования класса CTriangle
CTriangle::CTriangle(CTriangle const &other) : CPolygon(other) {}
// Конструктор перемещения класса CTriangle
CTriangle::CTriangle(CTriangle &&other) : CPolygon(std::move(other)) {}
// Оператор присваивания класса CTriangle
CTriangle & CTriangle::operator=(CTriangle const &other) {
    CPolygon::operator=(other);
    return *this;
}
// Перемещающий оператор присваивания класса CTriangle
CTriangle & CTriangle::operator=(CTriangle &&other) {
    CPolygon::operator=(std::move(other));
    return *this;
}
// Метод получения площади треугольника
double CTriangle::area() const {
    // Алгоритм использует формулу Герона
    std::vector<double> sides_v = sides();
    double perim_half = perimeter() / 2;
    return std::sqrt(perim_half * (perim_half - sides_v[0]) * (perim_half - sides_v[1]) * (perim_half - sides_v[2]));
}
// Метод получения типа треугольника (по углам) 
CTriangle::Triangle_types_angle CTriangle::angle_type() const {
    std::vector<double> angles_ = angles();
    for (double const &angle : angles_) {
        // Прямоугольный
        if (std::abs(angle - 90) <= EPS) {
            return Triangle_types_angle::RIGHT;
        }
        // Тупоугольный
        if (angle - 90 > EPS) {
            return Triangle_types_angle::OBTUSE;
        }
    }
    // Остроугольный
    return Triangle_types_angle::ACUTE;
}
// Метод получения типа треугольника (по длинам сторон)
CTriangle::Triangle_types_sides CTriangle::side_type() const {
    std::vector<double> sides_len = sides();
    // Равносторонний
    if (std::abs(sides_len[0] - sides_len[1]) <= EPS && std::abs(sides_len[0] - sides_len[2]) <= EPS) {
        return Triangle_types_sides::EQUILATERAL;
    }
    // Равнобедренный
    if (std::abs(sides_len[0] - sides_len[1]) <= EPS || std::abs(sides_len[0] - sides_len[2]) <= EPS || 
         std::abs(sides_len[1] - sides_len[2]) <= EPS){
        return Triangle_types_sides::ISOSCELES;
    }
    // Разносторонний
    return Triangle_types_sides::SCALENE;
}
// Метод получения длины медианы, проведённой к заданной стороне
double CTriangle::median(size_t i) const {
    std::vector<double> sides_len = sides();
    return std::sqrt(2 * std::pow(sides_len[i % 3], 2) +
                     2 * std::pow(sides_len[(i + 1) % 3], 2) - std::pow(sides_len[i - 1], 2)) / 2;
}
// Метод получения длины биссектрисы, проведённой к заданной стороне
double CTriangle::bisector(size_t i) const {
    double per_half = perimeter() / 2;
    std::vector<double> sides_len = sides();
    return (2 * std::sqrt(sides_len[i % 3] * sides_len[(i + 1) % 3] * per_half * (per_half - sides_len[i - 1]))) 
             / (sides_len[i % 3] + sides_len[(i + 1) % 3]);
}
// Метод получения длины высоты, проведённой к заданной стороне
double CTriangle::height(size_t i) const {
    CVector side(vertices_[i - 1], vertices_[i % 3]);
    return (2 * area()) / side.length();
}
// Метод получения радиуса вписанной окружности
double CTriangle::inscribed_radius() const {
    return (2 * area()) / perimeter();
}
// Метод получения радиуса описанной окружности
double CTriangle::circumscribed_radius() const {
    std::vector<double> sides_len = sides();
    double result = 1;
    for (double const &side : sides_len) {
        result *= side;
    }
    result /= (4 * area());
    return result;
}
// Треугольник всегда выпуклый
bool CTriangle::is_convex() const {
    return true;
}

// Перегрузка оператора вывода типа треугольника (по углу)
std::ostream& operator<<(std::ostream &os, CTriangle::Triangle_types_angle const &type) { 
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
// Перегрузка оператора вывода типа треугольника (по длинам сторон)
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


// Конструктор класса CTrapezoid от вектора точек
CTrapezoid::CTrapezoid(std::vector<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        // Если фигура не является трапецией по определению - ошибка
        if (!is_trapezoid()) {
            throw std::logic_error("Invalid trapezoid!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CTrapezoid от четырёх точек
CTrapezoid::CTrapezoid(CPoint const &a, CPoint const &b, CPoint const &c, CPoint const &d)
    : CPolygon({a, b, c, d}) {
    try {
        // Если фигура не является трапецией по определению - ошибка
        if (!is_trapezoid()) {
            throw std::logic_error("Invalid trapezoid!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CTrapezoid от initializer_list точек
CTrapezoid::CTrapezoid(std::initializer_list<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        // Если фигура не является трапецией по определению - ошибка
        if (!is_trapezoid()) {
            throw std::logic_error("Invalid trapezoid!");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор копирования класса CTrapezoid
CTrapezoid::CTrapezoid(CTrapezoid const &other) : CPolygon(other) {}
// Конструктор перемещения класса CTrapezoid
CTrapezoid::CTrapezoid(CTrapezoid &&other) : CPolygon(std::move(other)) {}
// Оператор присваивания класса CTrapezoid
CTrapezoid & CTrapezoid::operator=(CTrapezoid const &other) {
    CPolygon::operator=(other);
    return *this;
}
// Перемещающий оператор присваивания класса CTrapezoid
CTrapezoid & CTrapezoid::operator=(CTrapezoid &&other) {
    CPolygon::operator=(std::move(other));
    return *this;
}
// Метод получения площади трапеции
double CTrapezoid::area() const {
    // Алгоритм использует формулу вычисления площади через длины оснований и боковых сторон
    std::pair<double, double> bases_v = bases();
    std::pair<double, double> legs_v = legs();
    double a = std::min(bases_v.first, bases_v.second);
    double b = std::max(bases_v.first, bases_v.second);
    return (a + b) / (4 * (b - a)) * std::sqrt((a + legs_v.first + legs_v.second - b) * (a + legs_v.second - b - legs_v.first) 
             * (a + legs_v.first - b - legs_v.second) * (b + legs_v.first + legs_v.second - a));
}
// Проверка трапеции на равнобокость
bool CTrapezoid::is_isosceles() const {
    std::pair<double, double> legs_v = legs();
    return (std::abs(legs_v.first - legs_v.second) <= EPS);
}
// Проверка трапеции на прямоугольность
bool CTrapezoid::is_right() const {
    std::vector<double> angles_val = angles();
    return std::any_of(angles_val.begin(), angles_val.end(), [](double const &angle)
                       { return (std::abs(angle - 90) <= EPS); });
}
// Метод получения длины средней линии трапеции
double CTrapezoid::midsegment() const {
    std::pair<double, double> bases_v = bases();
    return (bases_v.first + bases_v.second) / 2;
}
// Метод получения высоты трапеции
double CTrapezoid::height() const {
    return area() / midsegment();
}
// Метод получения радиуса описанной окружности
double CTrapezoid::circumscribed_radius() const {
    if (is_isosceles()) {
        std::pair<double, double> bases_v = bases();
        std::pair<double, double> legs_v = legs();
        return std::sqrt((bases_v.first * bases_v.second + legs_v.first * legs_v.second) 
                 / (4 - std::pow((bases_v.first - bases_v.second) / legs_v.first, 2)));
    }
    else {
        return 0;
    }
}
// Метод получения радиуса вписанной окружности
double CTrapezoid::inscribed_radius() const {
    std::pair<double, double> bases_v = bases();
    std::pair<double, double> legs_v = legs();
    if (std::abs(bases_v.first + bases_v.second - legs_v.first + legs_v.second) <= EPS) {
        return height() / 2;
    }
    else {
        return 0;
    }
}
// Трапеция всегда выпкула
bool CTrapezoid::is_convex() const {
    return true;
}
// Метод получения длин оснований трапеции
std::pair<double, double> CTrapezoid::bases() const {
    CVector first_side(vertices_[0], vertices_[1]);
    CVector third_side(vertices_[2], vertices_[3]);
    if (std::abs(cross_prod_val(third_side, first_side)) <= EPS) {
        return std::make_pair(first_side.length(), third_side.length());
    }
    else {
        return std::make_pair(CVector(vertices_[1], vertices_[2]).length(), CVector(vertices_[3], vertices_[0]).length());
    }
}
// Метод получения длин боковых сторон трапеции
std::pair<double, double> CTrapezoid::legs() const {
    CVector first_side(vertices_[0], vertices_[1]);
    CVector third_side(vertices_[2], vertices_[3]);
    if (std::abs(cross_prod_val(third_side, first_side)) > EPS) {
        return std::make_pair(first_side.length(), third_side.length());
    }
    else {
        return std::make_pair(CVector(vertices_[1], vertices_[2]).length(), CVector(vertices_[3], vertices_[0]).length());
    }
}
// Проверка трапеции на корректность
bool CTrapezoid::is_trapezoid() const {
    // Не четыре вершины - не трапеция
    if (size() != 4) {
        return false;
    }
    CVector first_side(vertices_[0], vertices_[1]);
    CVector second_side(vertices_[1], vertices_[2]);
    CVector third_side(vertices_[2], vertices_[3]);
    CVector fourth_side(vertices_[3], vertices_[0]);
    // Две пары сторон трапеции параллельны, две другие - не параллельны
    if (std::abs(cross_prod_val(third_side, first_side)) <= EPS) {
        return std::abs(cross_prod_val(fourth_side, second_side)) > EPS;
    }
    return (std::abs(cross_prod_val(fourth_side, second_side)) <= EPS);
}


// Конструктор класса CRegular_polygon от вектора вершин
CRegular_polygon::CRegular_polygon(std::vector<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        // Многоугольник не является правильным - ошибка
        if (!((static_cast<CPolygon>(*this)).is_regular())) {
            throw std::logic_error("Given polygon is irregular");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CRegular_polygon от initializer_list точек
CRegular_polygon::CRegular_polygon(std::initializer_list<CPoint> const &vertices)
    : CPolygon(vertices) {
    try {
        // Многоугольник не является правильным - ошибка
        if (!((static_cast<CPolygon>(*this)).is_regular())) {
            throw std::logic_error("Given polygon is irregular");
        }
    }
    catch (const std::exception &e) {
        throw;
    }
}
// Конструктор класса CRegular_polygon от количества вершин, расстояния от центра до вершин, угла поворота и точки центра
CRegular_polygon::CRegular_polygon(size_t const &n, size_t const &radius,
                 double const &angle, CPoint const &center) {
    if (n >= 3) {
        std::vector<CPoint> edges;
        for (size_t i = 0; i < n; ++i) {
            double radian_angle = (angle + (360 * i) / n) * TO_RADIANS;
            double x_i = center.x() + radius * std::cos(radian_angle);
            double y_i = center.y() + radius * std::sin(radian_angle);
            edges.push_back(CPoint(x_i, y_i));
        }
        vertices_ = CClosed_polygonal_chain(edges);
    }
    else {
        try {
            // Правильный многоугольник имеет 3 и более вершины
            throw std::logic_error("Unable to construct such polygon");
        }
        catch (const std::exception &e) {
            throw;
        }
    }
}
// Конструктор копирования класса CRegular_polygon
CRegular_polygon::CRegular_polygon(CRegular_polygon const &other) : CPolygon(other) {}
// Конструктор перемещения класса CRegular_polygon
CRegular_polygon::CRegular_polygon(CRegular_polygon &&other) : CPolygon(std::move(other)) {}
// Оператор присваивания класса CRegular_polygon
CRegular_polygon & CRegular_polygon::operator=(CRegular_polygon const &other) {
    CPolygon::operator=(other);
    return *this;
}
// Перемещающий оператор присваивания класса CRegular_polygon
CRegular_polygon & CRegular_polygon::operator=(CRegular_polygon &&other) {
    CPolygon::operator=(std::move(other));
    return *this;
}
// Метод получения длины стороны правильного многоугольника
double CRegular_polygon::side() const {
    return CVector(vertices_[0], vertices_[1]).length();
}
// Метод получения вектора длин сторон правильного многоугольника
std::vector<double> CRegular_polygon::sides() const {
    return std::vector<double>(size(), side());
}
// Метод получения периметра правильного многоугольника
double CRegular_polygon::perimeter() const {
    return side() * size();
}
// Метод получения площади правильного многоугольника
double CRegular_polygon::area() const {
    return (perimeter() * side()) / (4 * std::tan(PI / size()));
}
// Метод получения значения угла правильного многоугольника в градусах
double CRegular_polygon::angle() const {
    return (static_cast<double>((size() - 2)) / size()) * 180;
}
// Метод получения вектора значений углов правильного многоугольника в градусах
std::vector<double> CRegular_polygon::angles() const {
    return std::vector<double>(size(), angle());
}
// Метод получения радиуса вписанной окружности
double CRegular_polygon::inscribed_radius() const {
    return side() / (2 * std::tan(PI / size()));
}
// Метод получения радиуса описанной окружности
double CRegular_polygon::circumscribed_radius() const {
    return side() / (2 * std::sin(PI / size()));
}
// Правильный многоугольник всегда выпуклый
bool CRegular_polygon::is_convex() const {
    return true;
}
// Правильный многоугольник всегда правильный
bool CRegular_polygon::is_regular() const {
    return true;
}