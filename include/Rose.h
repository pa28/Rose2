/*
 * Rose.h Created by Richard Buckley (C) 31/12/22
 */

#ifndef ROSE2_ROSE_H
#define ROSE2_ROSE_H

#include <SDL.h>
#include <type_traits>
#include <compare>
#include <iostream>
#include <fmt/format.h>

namespace rose {

    template<class T>
    concept StringLike = std::is_convertible_v<T, std::string_view>;

    enum class GadgetType {
        Gadget,
        Widget,
    };

    using ScreenCoordType = int;   ///< The type used to represent a screen coordinate.

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

        template<class I>
        requires std::is_integral_v<I>
        Size& operator=(I i) {
            w = i;
            h = i;
            return *this;
        }

        Size operator + (int border) {
            Size size{*this}; size.w += border; size.h += border; return size;
        }

        explicit operator bool () const { return set; }

        auto operator <=> (const Size &) const = default;
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

        template<class I>
        requires std::is_integral_v<I>
        Point& operator=(I i) {
            x = i;
            y = i;
            return *this;
        }

        auto operator <=> (const Point &) const = default;

        explicit operator bool () const { return set; }

        static Point CenterScreen(unsigned int screen = 0) {
            auto pos = static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(screen));
            return Point{pos, pos };
        }

        Point operator+(const Size& size) const {
            Point p{*this}; p.x += size.w; p.y += size.h; return p;
        }

        Point operator+(int border) const {
            Point p{*this}; p.x += border; p.y += border; return p;
        }

        Point operator+(const Point &p) const {
            Point r{*this}; r.x += p.x; r.y += p.y; return r;
        }
    };

    struct Padding {
        Point topLeft{};
        Point botRight{};

        Padding() = default;
        Padding(const Padding&) = default;
        Padding(Padding &&) = default;
        Padding& operator=(const Padding &) = default;
        Padding& operator=(Padding&&) = default;

        template<class I>
        requires std::is_integral_v<I>
        Padding& operator=(I i) {
            topLeft = i;
            botRight = i;
            return *this;
        }

        auto operator <=> (const Padding &) const = default;

        explicit operator bool () const { return topLeft && botRight; }

        ~Padding() = default;
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

        /**
         * @brief Construct a Rectangle from a Point and a Size.
         * @param point The Point.
         * @param size The Size.
         */
        [[maybe_unused]] Rectangle(const Point& point, const Size &size) : point(point), size(size) {}

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

        [[nodiscard]] Rectangle intersection(const Rectangle &o) const {
            // gives bottom-left point
            // of intersection rectangle

            auto x5 = std::max(point.x, o.point.x);
            auto y5 = std::max(point.y, o.point.y);

            // gives top-right point
            // of intersection rectangle
            auto x6 = std::min(point.x+size.w, o.point.x+o.size.w);
            auto y6 = std::min(point.y+size.h, o.point.y+o.size.h);

            // no intersection
            if (x5 > x6 || y5 > y6) {
                return Rectangle{0,0,0,0};
            }

            return Rectangle{x5, y5, x6 - x5, y6 - y5};
        }
    };
} // rose


inline rose::Size operator + (const rose::Size &s, const rose::Point& p) {
    rose::Size r{s}; r.w += p.x; r.h += p.y; return r;
}

namespace fmt {
    template<>
    class formatter<rose::Point> {
        char presentation_ = 'd';
    public:
        // parse format and store it
        constexpr auto parse(format_parse_context &ctx) {
            auto i = ctx.begin(), end = ctx.end();
            if (i != end && (*i == 'd' || *i == 'o')) {
                presentation_ = *i++;
            }
            if (i != end && *i != '}') {
                throw format_error("Invalid rectangle format.");
            }
            return i;
        }

        // format a value using stored specification:
        template<class FmtContext>
        constexpr auto format(const rose::Point &p, FmtContext &ctx) const {
            // note: we can't use ternary operator '?:' in a constexpr
            switch (presentation_) {
                default:
                    // 'ctx.out()' is an output iterator
                case 'd':
                    return format_to(ctx.out(), "[{:d},{:d}]", p.x, p.y);
            }
        }
    };

    template<>
    class formatter<rose::Size> {
        char presentation_ = 'd';
    public:
        // parse format and store it
        constexpr auto parse(format_parse_context &ctx) {
            auto i = ctx.begin(), end = ctx.end();
            if (i != end && (*i == 'd' || *i == 'o')) {
                presentation_ = *i++;
            }
            if (i != end && *i != '}') {
                throw format_error("Invalid rectangle format.");
            }
            return i;
        }

        // format a value using stored specification:
        template<class FmtContext>
        constexpr auto format(const rose::Size &s, FmtContext &ctx) const {
            // note: we can't use ternary operator '?:' in a constexpr
            switch (presentation_) {
                default:
                    // 'ctx.out()' is an output iterator
                case 'd':
                    return format_to(ctx.out(), "[{:d},{:d}]", s.w, s.h);
            }
        }
    };

    template<>
    class formatter<rose::Rectangle> {
        char presentation_ = 'd';
    public:
        // parse format and store it
        constexpr auto parse(format_parse_context &ctx) {
            auto i = ctx.begin(), end = ctx.end();
            if (i != end && (*i == 'd' || *i == 'o')) {
                presentation_ = *i++;
            }
            if (i != end && *i != '}') {
                throw format_error("Invalid rectangle format.");
            }
            return i;
        }

        // format a value using stored specification:
        template<class FmtContext>
        constexpr auto format(const rose::Rectangle &r, FmtContext &ctx) const {
            // note: we can't use ternary operator '?:' in a constexpr
            switch (presentation_) {
                default:
                    // 'ctx.out()' is an output iterator
                case 'd':
                    return format_to(ctx.out(), "[{:d},{:d},{:d},{:d}]", r.point.x, r.point.y, r.size.w, r.size.h);
            }
        }
    };
}

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

inline std::ostream &operator<<(std::ostream& ostream, const rose::Rectangle &r) {
    return ostream << '[' << r.point.x << ',' << r.point.y << ',' << r.size.w << ',' << r.size.h << ']';
}

#endif //ROSE2_ROSE_H
