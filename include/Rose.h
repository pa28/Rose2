/*
 * Rose.h Created by Richard Buckley (C) 31/12/22
 */

#ifndef ROSE2_ROSE_H
#define ROSE2_ROSE_H

#include <type_traits>
#include <compare>
#include <iostream>

namespace rose {

    enum class GadgetType {
        Gadget,
        Widget,
    };

    using ScreenCoordType = long;   ///< The type used to represent a screen coordinate.

    struct Size {
        ScreenCoordType w{0}, h{0};
        bool set{false};

        Size() = default;
        Size(const Size&) = default;
        Size(Size&&) = default;
        Size& operator=(const Size &) = default;
        Size& operator=(Size &&) = default;

        template<typename Tw, typename Th>
        Size(Tw W, Th H) : w(W), h(H), set(true) {
            static_assert(std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                    "Arguments to Size() must be convertable to ScreenCoordType");
        }

        ~Size() = default;

        auto operator <=> (const Size &) const = default;

        explicit operator bool () const { return set; }
    };

    /**
     * @class Point
     * @brief Representation of a location on a screen or other drawable.
     */
    struct Point {
        ScreenCoordType x{0}, y{0};
        bool set{false};

        Point() = default;
        Point(const Point&) = default;
        Point(Point &&) = default;
        Point& operator=(const Point &) = default;
        Point& operator=(Point&&) = default;

        template<typename Tx, typename Ty>
        Point(Tx X, Ty Y) : x(X), y(Y), set(true) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType>,
                          "Arguments to Point() must be convertable to ScreenCoordType");
        }

        ~Point() = default;

        auto operator <=> (const Point &) const = default;

        explicit operator bool () const { return set; }

        Point& operator += (const Size& size) {
            x += size.w;
            y += size.h;
            return *this;
        }

        Point& operator -= (const Size& size) {
            x -= size.w;
            y -= size.h;
            return *this;
        }

        Point operator + (const Size& size) {
            return Point(*this) += size;
        }

        Point operator - (const Size& size) {
            return Point(*this) -= size;
        }
    };

    struct Rectangle {
        Point point{};
        Size size{};

        Rectangle() = default;
        Rectangle(const Rectangle&) = default;
        Rectangle(Rectangle&&) = default;
        Rectangle& operator=(const Rectangle &) = default;
        Rectangle& operator=(Rectangle &&) = default;

        template<typename Tx, typename Ty, typename Tw, typename Th>
        [[maybe_unused]] Rectangle(Tx X, Ty Y, Tw W, Th H) : point(X,Y), size(W,H) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType> &&
                    std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                    "Arguments to Size() must be convertable to ScreenCoordType");
        }

        Rectangle& operator=(const Point &p) {
            point = p;
            return *this;
        }

        Rectangle& operator=(const Size &s) {
            size = s;
            return *this;
        }

        auto operator <=> (const Rectangle &) const = default;

        auto operator <=> (const Point &p) const {
            return point <=> p;
        }

        auto operator <=> (const Size &s) const {
            return size <=> s;
        }

        explicit operator bool () const { return point.set && size.set; }

        [[maybe_unused]] bool contains(const Point &p) {
            return p >= point && p < (point + size);
        }
    };
} // rose

inline rose::Size operator - (const rose::Point &p0, const rose::Point &p1) {
    return {p0.x - p1.x, p0.y - p1.y};
}

inline auto operator <=> (const rose::Point &p, const rose::Rectangle &r) {
    return p.operator<=>(r.point);
}

inline auto operator <=> (const rose::Size &s, const rose::Rectangle &r) {
    return s.operator<=>(r.size);
}

inline std::ostream &operator<<(std::ostream& ostream, const rose::Point &p) {
    return ostream << '[' << p.x << ',' << p.y << ']';
}

inline std::ostream &operator<<(std::ostream& ostream, const rose::Size &s) {
    return ostream << '[' << s.w << ',' << s.h << ']';
}

#endif //ROSE2_ROSE_H
