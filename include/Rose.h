#ifndef ROSE2_ROSE_H
#define ROSE2_ROSE_H

#include <type_traits>
#include <compare>
#include <iostream>

namespace rose {

    using ScreenCoordType = long;   ///< The type used to represent a screen coordinate.

    /**
     * @class Point
     * @brief Representation of a location on a screen or other drawable.
     */
    struct Point {
        ScreenCoordType x{0}, y{0};

        Point() = default;
        Point(const Point&) = default;
        Point(Point &&) = default;
        Point& operator=(const Point &) = default;
        Point& operator=(Point&&) = default;

        template<typename Tx, typename Ty>
        Point(Tx X, Ty Y) : x(X), y(Y) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType>,
                          "Arguments to Point() must be convertable to ScreenCoordType");
        }

        ~Point() = default;

        auto operator <=> (const Point &) const = default;
    };

    struct Size {
        ScreenCoordType w{0}, h{0};

        Size() = default;
        Size(const Size&) = default;
        Size(Size&&) = default;
        Size& operator=(const Size &) = default;
        Size& operator=(Size &&) = default;

        template<typename Tw, typename Th>
        Size(Tw W, Th H) : w(W), h(H) {
            static_assert(std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                    "Arguments to Size() must be convertable to ScreenCoordType");
        }

        ~Size() = default;

        auto operator <=> (const Size &) const = default;
    };

    struct Rectangle {
        ScreenCoordType x{0}, y{0}, w{0}, h{0};

        Rectangle() = default;
        Rectangle(const Rectangle&) = default;
        Rectangle(Rectangle&&) = default;
        Rectangle& operator=(const Rectangle &) = default;
        Rectangle& operator=(Rectangle &&) = default;

        template<typename Tx, typename Ty, typename Tw, typename Th>
        Rectangle(Tx X, Ty Y, Tw W, Th H) : x(X), y(Y), w(W), h(H) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType> &&
                    std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                    "Arguments to Size() must be convertable to ScreenCoordType");
        }

        Rectangle& operator=(const Point &p) {
            x = p.x, y = p.y;
            return *this;
        }

        Rectangle& operator=(const Size &s) {
            w = s.w, h = s.h;
            return *this;
        }

        auto operator <=> (const Rectangle &) const = default;

        auto operator <=> (const Point &p) const {
            return Point(x,y) <=> p;
        }

        auto operator <=> (const Size &s) const {
            return Size(w, h) <=> s;
        }
    };
} // rose

rose::Size operator - (const rose::Point &p0, const rose::Point &p1) {
    return {p0.x - p1.x, p0.y - p1.y};
}

auto operator <=> (const rose::Point &p, const rose::Rectangle &r) {
    return p.operator<=>(rose::Point(r.x, r.y));
}

auto operator <=> (const rose::Size &s, const rose::Rectangle &r) {
    return s.operator<=>(rose::Size(r.w, r.h));
}

std::ostream &operator<<(std::ostream& ostream, const rose::Point &p) {
    return ostream << '[' << p.x << ',' << p.y << ']';
}

std::ostream &operator<<(std::ostream& ostream, const rose::Size &s) {
    return ostream << '[' << s.w << ',' << s.h << ']';
}

#endif //ROSE2_ROSE_H
