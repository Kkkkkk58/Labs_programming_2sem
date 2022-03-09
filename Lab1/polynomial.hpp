#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <complex>
#include <type_traits>
#include <regex>

constexpr double EPS = 1e-9;        // Константа времени исполнения "эпсилон" для выполнения сравнения чисел с плавающей точкой
const double PI = std::acos(-1.L);  // Математическая константа - число пи

// Шаблонный класс полинома
template <class T, class E = void>
class CPolynomial;

// Основные вспомогательные функции
namespace Polynomial_helper {

    // Метафункция, имеющая значение true, если шаблонный параметр - объект класса std::complex, false - в противном случае
    template<class T>
    struct is_complex : public std::false_type {};
    // Частная специализация метафункции, имеющая значение true для типа комплексных чисел
    template<class T>
    struct is_complex<std::complex<T>> : public std::true_type {};

    // Шаблонная функция проверки на равенство нулю
    template<class T>
    bool equals_zero(T const &value) {
        // Если передано число с плавающей точкой - сравниваем с "эпсилон", иначе с T(0)
        return (std::is_floating_point<T>::value && std::abs(value) <= EPS) ||
         (!std::is_floating_point<T>::value && value == T(0));
    }

    // Шаблонная функция проверки на отрицательность
    template<class T>
    bool less_than_zero(T const &value) {
        // Если передано число с плавающей точкой - сравниваем с "эпсилон", иначе с T(0)
        return (std::is_floating_point<T>::value && value < EPS) ||
        (!std::is_floating_point<T>::value && value < T(0));
    }
    // Рекурсивная версия быстрого преобразования Фурье для перемножения двух полиномов
    void fft(std::vector<std::complex<double>> &compl_coefs, bool invert) {
        size_t degree = compl_coefs.size();
        // Если полученная на одном из шагов степень части полинома опускается до 1, выходим из рекурсии
        if (degree <= 1) {
            return;
        }
        // Деление полинома на две части (с четными и нечетными коэффициентами) и заполнение соответствующими копмлексными значениями
        std::vector<std::complex<double>> first_part(degree / 2);
        std::vector<std::complex<double>> second_part(degree / 2);
        for (int i = 0; i < degree; i += 2) {
            first_part[i / 2] = compl_coefs[i];
            second_part[i / 2] = compl_coefs[i + 1];
        }
        // Рекурсивный запуск для обеих частей
        fft(first_part, invert);
        fft(second_part, invert);
        // Аргумент комплексного числа (угол)
        double argument = 2 * PI / degree * (invert ? -1 : 1);
        // n-ый комплексный корень, равный единице
        std::complex<double> root(1);
        // Значение, на которое умножамем корни для получения следующих (из формулы Эйлера)
        std::complex<double> multiplier(std::cos(argument), std::sin(argument));
        for (int i = 0; i < degree / 2; ++i) {
            compl_coefs[i] = first_part[i] + root * second_part[i];
            compl_coefs[i + degree / 2] = first_part[i] - root * second_part[i];
            // Если требуется провести обратное преобразование
            if (invert) {
                compl_coefs[i] /= 2;
                compl_coefs[i + degree / 2] /= 2;
            }
            root *= multiplier;
        }
    }

    // Функция, заполняющая соответствующий степени коэффициент
    template<class T>
    void fill_coefficient(std::map<size_t, T> &coefficients, size_t degree, T const &value) {
        // Если при заданной степени уже есть коэффициент - складываем
        if (coefficients.find(degree) != coefficients.end()) {
            // Если сумма не равна нулю, записываем
            if (!Polynomial_helper::equals_zero(coefficients[degree] + value)) {
                coefficients[degree] += value;
            }
            // Иначе удаляем
            else {
                coefficients.erase(degree);
            }
        }
        else {
            coefficients[degree] = value;
        }
    }

    // Функция, возвращающая степень из строки
    size_t get_degree(std::string const &degree_holder, std::string const &whole_str, std::string const &numeric_part) {
        if (degree_holder.length()) {
            return stoi(degree_holder);
        }
        if (whole_str != numeric_part) {
            return 1;
        }
        else {
            return 0;
        }
    }


    // Шаблонная функция для получения целочисленных коэффициентов многочлена из строки
    template<class T>
    typename std::enable_if<std::is_integral<T>::value, std::map<size_t, T>>::type
    from_string(std::string const &str) {
        // Регулярное выражение для записи целочисленных коэффициентов
        std::regex poly_template("\\+|\\-?(\\d+)?\\*?[a-zA-Z]?\\^?(\\d+)?");
        std::sregex_iterator rit(str.begin(), str.end(), poly_template);
        std::sregex_iterator rend;
        std::map<size_t, T> coefficients;
        // Пока есть совпадения, обрабатываем их
        while (rit != rend) {
            if (std::string((*rit)[0]) != "+" && std::string((*rit)[0]).length()) {
                T value(0);        // Сам коэффициент
                int sign = 1;      // Знак коэффициента
                size_t degree = 0; // Степень при коэффициенте
                // Получение знака
                if (std::string((*rit)[0])[0] == '-') {
                    sign = -1;
                }
                // Получение значения
                if (std::string((*rit)[1]).length()) {
                    value = sign * std::stoi(std::string((*rit)[1]));
                }
                else {
                    value = sign * T(1);
                }
                if (value == T(0)) {
                    ++rit;
                    continue;
                }
                // Получение степени
                degree = get_degree(std::string((*rit)[2]), std::string((*rit)[0]), std::string((*rit)[1]));
                // Вставляем значение в соответствующее место в ряду коэффициентов
                fill_coefficient<T>(coefficients, degree, value);
            }
            ++rit;
        }
        return coefficients;
    }

    // Шаблонная функция для получения коэффициентов с плавающей точкой многочлена из строки
    template<class T>
    typename std::enable_if<std::is_floating_point<T>::value, std::map<size_t, T>>::type
    from_string(std::string const &str) {
        // Регулярное выражение для записи коэффициентов с плавающей точкой
        std::regex poly_template("\\+|\\-?(\\d*\\.)?(\\d+)?\\*?[a-zA-Z]?\\^?(\\d+)?");
        std::sregex_iterator rit(str.begin(), str.end(), poly_template);
        std::sregex_iterator rend;
        std::map<size_t, T> coefficients;
        while (rit != rend) {
            if (std::string((*rit)[0]) != "+" && std::string((*rit)[0]).length()) {
                T value(0);        // Сам коэффициент
                int sign = 1;      // Знак коэффициента
                size_t degree = 0; // Степень при коэффициенте
                // Получение знака
                if (std::string((*rit)[0])[0] == '-') {
                    sign = -1;
                }
                // Получение значения
                std::string num = std::string((*rit)[1]) + std::string((*rit)[2]);
                if (num.length()) {
                    value = sign * std::stod(num);
                }
                else {
                    value = sign * T(1);
                }
                if (value <= EPS) {
                    ++rit;
                    continue;
                }
                // Получение степени
                degree = get_degree(std::string((*rit)[3]), std::string((*rit)[0]), num);
                // Вставляем значение в соответствующее место в ряду коэффициентов
                fill_coefficient<T>(coefficients, degree, value);
            }
            ++rit;
        }
        return coefficients;
    }

    // Шаблонная функция для получения комплексных коэффициентов многочлена из строки
    template<class T>
    typename std::enable_if<is_complex<T>::value, std::map<size_t, T>>::type
    from_string(std::string const &str) {
        // Регулярное выражение для записи комплексных коэффициентов
        std::regex poly_template("\\+|\\-?(\\((\\d*\\.)?(\\d+)?\\,?(\\d*\\.)?(\\d+)?\\))?\\*?[a-zA-Z]?\\^?(\\d+)?");
        std::sregex_iterator rit(str.begin(), str.end(), poly_template);
        std::sregex_iterator rend;
        std::map<size_t, T> coefficients;
        while (rit != rend) {
            if (std::string((*rit)[0]) != "+" && std::string((*rit)[0]).length()) {
                T value(0);        // Сам коэффициент
                int sign = 1;      // Знак коэффициента
                size_t degree = 0; // Степень при коэффициенте
                // Получение знака
                if (std::string((*rit)[0])[0] == '-') {
                    sign = -1;
                }
                // Получение значения
                if (std::string((*rit)[1]).length()) {
                    std::string re_str = std::string((*rit)[2]) + std::string((*rit)[3]);
                    double re = 0;
                    if (re_str.length()) {
                        re = std::stod(re_str);
                    }
                    std::string im_str = std::string((*rit)[4]) + std::string((*rit)[5]);
                    double im = 0;
                    if (im_str.length()) {
                        im = std::stod(im_str);
                    }
                    value = T(re, im);
                    value *= sign;
                }
                else {
                    value = T(1);
                    value *= sign;
                }
                if (value == T(0)) {
                    ++rit;
                    continue;
                }
                // Получение степени
                degree = get_degree(std::string((*rit)[6]), std::string((*rit)[0]), std::string((*rit)[1]));
                // Вставляем значение в соответствующее место в ряду коэффициентов
                fill_coefficient<T>(coefficients, degree, value);
            }
            ++rit;
        }
        return coefficients;
    }
}

// Частичная специализация класса полинома для заданных типов коэффициентов - целочисленных, с плавающей точкой, комплексных
template <class T>
class CPolynomial<T, typename std::enable_if<std::is_arithmetic<T>::value || \
Polynomial_helper::is_complex<T>::value>::type> {
public:
    // Конструктор класса CPolynomial от вектора коэффициентов
    explicit CPolynomial(std::vector<T> const &coefficients = std::vector<T>()) {
        for (size_t i = 0; i < coefficients.size(); ++i) {
            // Нулевые коэффициенты не добавляются в полином
            if (!Polynomial_helper::equals_zero(coefficients[i])) {
                coefficients_[i] = coefficients[i];
            }
        }
    }
    // Конструктор класса CPolynomial от map, содержащей степени и коэффициенты
    explicit CPolynomial(std::map<size_t, T> const &coefficients) : coefficients_(coefficients) {
        auto iter = coefficients_.begin();
        while (iter != coefficients_.end()) {
            // Нулевые коэффициенты удаляются из полинома
            if (Polynomial_helper::equals_zero(iter->second)) {
                iter = coefficients_.erase(iter);
            }
            else {
                ++iter;
            }
        }
    }
    // Конструктор класса CPolynomial от initializer_list
    explicit CPolynomial(std::initializer_list<T> const &coefficients) {
        size_t i = 0;
        for (auto iter = coefficients.begin(); iter != coefficients.end(); ++iter, ++i) {
            // Нулевые значения не добавляются
            if (!Polynomial_helper::equals_zero(*iter)) {
                coefficients_[i] = *iter;
            }
        }
    }
    // Конструктор класса CPolynomial от количества коэффициентов, равных второму передаваемому аргументу
    explicit CPolynomial(size_t size, T const &value = T(0)) {
        if (!Polynomial_helper::equals_zero(value)) {
            for (size_t i = 0; i <= size; ++i) {
                coefficients_[i] = value;
            }
        }
        // Полином степени size не может иметь size-тым коэффициентом ноль
        else {
            coefficients_[size] = T(1);
        }
    }
    // Констуктор класса CPolynomial от строки
    explicit CPolynomial(std::string str) : coefficients_(std::map<size_t, T>()) {
        // Удаление пробелов из копии строки для удобной работы
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        coefficients_ = Polynomial_helper::from_string<T>(str);
    }
    // Деструктор класса CPolynomial
    ~CPolynomial() = default;
    // Конструктор копирования класса CPolynomial
    CPolynomial(CPolynomial const &other) : coefficients_(other.coefficients_) {}
    // Метод swap класса CPolynomial для обмена коэффициентами полиномов
    void swap(CPolynomial &other) {
        std::swap(coefficients_, other.coefficients_);
    } 
    // Конструктор перемещения класса CPolynomial
    CPolynomial(CPolynomial &&other) { swap(other); }
    // Оператор присваивания класса CPolynomial
    CPolynomial &operator=(CPolynomial const &other) {
        if (this != &other) {
            coefficients_ = other.coefficients_;
        }
        return *this;
    }
    // Перемещающий оператор присваивания класса CPolynomial
    CPolynomial &operator=(CPolynomial &&other) {
        swap(other);
        return *this;
    }
    // Метод получения степени полинома
    int deg() const {
        if (!coefficients_.empty()) {
            return coefficients_.rbegin()->first;
        }
        // Степень полинома, имеющего только нулевые коэффициенты принято считать -1 или -inf
        else {
            return -1;
        }
    }
    // Проверка полиномов на равенство
    bool operator==(CPolynomial const &other) const {
        return coefficients_ == other.coefficients_;
    }
    // Проверка полиномов на неравенство
    bool operator!=(CPolynomial const &other) const {
        return !(*this == other);
    }
    // Проверка полиномов на выполнение отношения "<"
    bool operator<(CPolynomial const &other) const {
        return coefficients_ < other.coefficients_;
    }
    // Проверка полиномов на выполнение отношения ">"
    bool operator>(CPolynomial const &other) const {
        return other < *this;
    }
    // Проверка полиномов на выполнение отношения "<="
    bool operator<=(CPolynomial const &other) const {
        return !(other < *this);
    }
    // Проверка полиномов на выполнение отношения ">="
    bool operator>=(CPolynomial const &other) const {
        return !(*this < other);
    }
    // Унарный оператор "+"
    CPolynomial operator+() const {
        return *this;
    }
    // Унарный оператор "-"
    CPolynomial operator-() const {
        std::map<size_t, T> new_coefficients;
        for (size_t i = 0; i <= deg(); ++i) {
            auto finder = coefficients_.find(i);
            if (finder != coefficients_.end()) {
                new_coefficients[i] = -(finder->second);
            }
        }
        return CPolynomial(new_coefficients);
    }
    // Оператор "+="
    CPolynomial &operator+=(CPolynomial const &other) {
        for (size_t i = 0; i <= std::max(deg(), other.deg()); ++i) {
            auto this_finder = coefficients_.find(i);
            auto other_finder = other.coefficients_.find(i);
            // Если оба полинома имеют коэффициенты при степени i
            if (this_finder != coefficients_.end() && other_finder != other.coefficients_.end()) {
                // Если коэффициенты не являются противоположными - выполняем их сложкение
                if (!Polynomial_helper::equals_zero(this_finder->second + other_finder->second)) {
                    coefficients_[i] += other_finder->second;
                }
                // Иначе удаляем коэффициент
                else {
                    coefficients_.erase(i);
                }
            }
            // Если только добавляемый полином имеет коэффициент при степени i, заполняем его
            else if (this_finder == coefficients_.end() && other_finder != other.coefficients_.end())
            {
                coefficients_[i] = other_finder->second;
            }
        }
        return *this;
    }
    // Оператор "-="
    CPolynomial &operator-=(CPolynomial const &other) {
        *this += (-other);
        return *this;
    }
    // Оператор "*=" на число типа T
    CPolynomial &operator*=(T const &value) {
        // Если число не равно нулю, честно умножаем
        if (!Polynomial_helper::equals_zero(value)) {
            for (auto coef : coefficients_) {
                coefficients_[coef.first] = value * coef.second;
            }
        }
        // Иначе ставим полиному в соотвествие пустой набор коэффициентов
        else {
            std::map<size_t, T> empty_coefficients;
            coefficients_ = empty_coefficients;
        }
        return *this;
    }
    // Оператор "*=" на другой полином
    CPolynomial &operator*=(CPolynomial const &other) {
        // Преобразуем коэффициенты первого многочлена в комплексные для работы с преобразованием Фурье
        std::vector<std::complex<double>> compl_first(deg() + 1);
        for (size_t i = 0; i <= deg(); ++i) {
            if (coefficients_.find(i) != coefficients_.end()) {
                compl_first[i] = std::complex<double>(coefficients_.at(i));
            }
        }
        // Преобразуем коэффициенты второго многочлена в комплексные для работы с преобразованием Фурье
        std::vector<std::complex<double>> compl_second(other.deg() + 1);
        for (size_t i = 0; i <= other.deg(); ++i) {
            if (other.coefficients_.find(i) != other.coefficients_.end()) {
                compl_second[i] = std::complex<double>(other.coefficients_.at(i));
            }
        }
        // Для работы с преобразованием Фурье полиномы должны иметь равные степени, равные степени двойки
        size_t degree = 1;
        while (degree < std::max(compl_first.size(), compl_second.size())) {
            degree <<= 1;
        }
        degree <<= 1;
        compl_first.resize(degree);
        compl_second.resize(degree);
        // Вызываем прямое преобразование Фурье для обоих наборов коэффициентов
        Polynomial_helper::fft(compl_first, false);
        Polynomial_helper::fft(compl_second, false);
        // Умножаем полученные первые коэффициенты на вторые
        for (size_t i = 0; i < degree; ++i) {
            compl_first[i] *= compl_second[i];
        }
        // Проводим обратное преобразование Фурье, работая с первыми коэффициентами
        Polynomial_helper::fft(compl_first, true);
        // Результирующий вектор содержит вещественные части полученных после преобразования коэффициентов
        std::vector<T> res(degree); 
        for (size_t i = 0; i < degree; ++i) {
            res[i] = compl_first[i].real();
        }
        // Строим полином от полученного вектора
        *this = CPolynomial(res);
        return *this;
    }
    // Оператор "/=" на число типа T
    CPolynomial &operator/=(T const &value) {
        // Если число не равно нулю
        if (!Polynomial_helper::equals_zero(value)) {
            for (auto coef = coefficients_.begin(); coef != coefficients_.end(); ) {
                // Если коэффициент, полученный в результате деления не будет преобразован к нулю - записываем его
                if (!Polynomial_helper::equals_zero(coef->second / value)) {
                    coef->second /= value;
                    ++coef;
                }
                // Иначе удаляем
                else {
                    coef = coefficients_.erase(coef);
                }
            }
        }
        // Иначе будем считать, что получаем "пустой" полином
        else {
            std::map<size_t, T> empty_coefficients;
            coefficients_ = empty_coefficients;
        }
        
        return *this;
    }
    // Оператор "/=" на другой полином
    CPolynomial &operator/=(CPolynomial const &other) {
        CPolynomial res;
        // Деление полиномов "в столбик"
        while (deg() >= other.deg()) {
            size_t res_deg = deg() - other.deg();
            CPolynomial multiplier(res_deg);
            CPolynomial sub = other * multiplier;
            // Если старшие коэффициенты различны по знаку - не вычитаем, а складываем
            if (Polynomial_helper::less_than_zero<T>(other[other.deg()] * operator[](deg()))) {
                    sub *= -1;
                    multiplier *= -1;
                }
            res += multiplier;
            *this -= sub;
        }
        *this = res;
        return *this;
    }
    // Оператор "%=" на другой полином
    CPolynomial &operator%=(CPolynomial const &other) {
        // Деление "в столбик" без запоминания результирующего полинома
        while (deg() >= other.deg()) {
            size_t res_deg = deg() - other.deg();
            CPolynomial sub = other * CPolynomial(res_deg);
            // Если старшие коэффициенты различны по знаку - не вычитаем, а складываем
            if (Polynomial_helper::less_than_zero<T>(other[other.deg()] * operator[](deg()))) {
                sub *= -1;
            }
            *this -= sub;
        }
        return *this;
    }
    // Оператор [] для получения значения коэффициента при определённой степени
    T operator[](size_t i) const {
        auto coef = coefficients_.find(i);
        if (coef != coefficients_.end()) {
            return coef->second;
        }
        // Если коэффициента при степени i в полиноме не нашлось, то он равен нулю
        return T(0);
    }
    // Перегрузка оператора вывода в поток для класса CPolynomial
    friend std::ostream& operator<<(std::ostream &os, CPolynomial const &p) { 
        // Если имеем полином степени -1
        if (p.coefficients_.size() == 0) {
            os << T(0);
        }
        else {
            for (auto degree : p.coefficients_) {
                // При нулевой степени не печатаем "x^0" и "+"
                if (degree.first == 0) {
                    os << degree.second;
                }
                // Если степень не нулевая, но коэффициент - первый в списке, перед ним не печатаем "+"
                else if (degree == *p.coefficients_.begin()) {
                    // Не печатаем "^1" при первой степени
                    if (degree.first == 1) {
                        os << degree.second << "x";
                    }
                    else {
                        os << degree.second << "x^" << degree.first;
                    }
                }
                else {
                    // Не печатаем "^1" при первой степени
                    if (degree.first == 1) {
                        os << " + " << degree.second << "x";
                    }
                    else {
                        os << " + " << degree.second << "x^" << degree.first;
                    }
                }
            }
        }
        return os; 
    }
    // Перегрузка оператора ввода из потока для класса CPolynomial
    friend std::istream& operator>>(std::istream &is, CPolynomial &p) {
        size_t i = 0;
        T coef;
        is >> coef;
        // Пока поток не вывставил флаги badbit || failbit, читаем и заполняем последовательно коэффициенты
        while (is.good()) {
            if (!Polynomial_helper::equals_zero(coef)) {
                p.coefficients_[i] = coef;
            }
            ++i;
            is >> coef;
        }
        return is;
    }
    // Функция для получения значения полинома в какой-то точке
    T evaluation(T const &argument) const {
        if (!coefficients_.empty()) {
            // Алгоритм использует схему Горнера
            T eval = coefficients_.rbegin()->second;
            for (int i = deg() - 1; i >= 0; --i) {
                eval *= argument;
                if (coefficients_.find(i) != coefficients_.end()) {
                    eval += coefficients_.at(i);
                }
            }
            return eval;
        }
        else {
            return 0;
        }
    }
    // Функция расчёта производной полинома порядка order
    friend CPolynomial derivative(CPolynomial poly, size_t const &order) {
        // Последовательно берём соответствующие производные
        for (size_t i = 0; i < order; ++i) {
            // Если на каком-то шаге получили 0, можем не дифференцировать дальше
            if (poly.deg() != -1) {
                // Дифференцируем
                auto curr = poly.coefficients_.erase(poly.coefficients_.find(0));
                while (curr != poly.coefficients_.end()) {
                    poly.coefficients_[curr->first - 1] = static_cast<T>(curr->first) * curr->second;
                    curr = poly.coefficients_.erase(curr);
                }
            }
            else {
                break;
            }
        }  
        return poly;
    }
private:
    std::map<size_t, T> coefficients_;      // map, где ключ - номер степени, значение - коэффициент при соответствующей степени
};

// Бинарный оператор сложения для класса CPolynomial
template<class T>
CPolynomial<T> operator+(CPolynomial<T> lhs, CPolynomial<T> const &rhs) {
    return lhs += rhs;
}

// Бинарный оператор вычитания для класса CPolynomial
template<class T>
CPolynomial<T> operator-(CPolynomial<T> lhs, CPolynomial<T> const &rhs) {
    return lhs -= rhs;
}

// Бинарный оператор умножения на число справа для класса CPolynomial
template<class T>
CPolynomial<T> operator*(CPolynomial<T> lhs, double rhs) {
    return lhs *= rhs;
}

// Бинарный оператор умножения на число слева для класса CPolynomial
template<class T>
CPolynomial<T> operator*(double const &lhs, CPolynomial<T> rhs) {
    return rhs *= lhs;
}

// Бинарный оператор деления на число для класса CPolynomial
template<class T>
CPolynomial<T> operator/(CPolynomial<T> lhs, double rhs) {
    return lhs /= rhs;
}

// Бинарный оператор умножения на полином для класса CPolynomial
template<class T>
CPolynomial<T> operator*(CPolynomial<T> lhs, CPolynomial<T> const &rhs) {
    return lhs *= rhs;
}

// Бинарный оператор целочисленного деления на полином для класса CPolynomial
template<class T>
CPolynomial<T> operator/(CPolynomial<T> lhs, CPolynomial<T> const &rhs) {
    return lhs /= rhs;
}

// Бинарный оператор взятия остатка от деления на полином для класса CPolynomial
template<class T>
CPolynomial<T> operator%(CPolynomial<T> lhs, CPolynomial<T> const &rhs) {
    return lhs %= rhs;
}

#endif
