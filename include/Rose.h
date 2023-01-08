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

        constexpr Size() = default;
        constexpr Size(const Size&) = default;
        constexpr Size(Size&&) = default;
        constexpr Size& operator=(const Size &) = default;
        constexpr Size& operator=(Size &&) = default;

        template<typename Tw, typename Th>
        constexpr Size(Tw W, Th H) : w(W), h(H), set(true) {
            static_assert(std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                    "Arguments to Size() must be convertable to ScreenCoordType");
        }

        template<typename Ts>
        requires std::is_convertible_v<Ts, ScreenCoordType>
        constexpr explicit Size(Ts s) : w(s), h(s), set(true) {}

        ~Size() = default;

        template<class I>
        requires std::is_integral_v<I>
        constexpr Size& operator=(I i) {
            w = i;
            h = i;
            return *this;
        }

        constexpr Size operator + (int border) {
            Size size{*this}; size.w += border; size.h += border; return size;
        }

        constexpr Size operator + (const Size& size) const {
            return Size{w + size.w, h + size.h};
        }

        constexpr explicit operator bool () const { return set; }

        auto operator <=> (const Size &) const = default;
    };

    /**
     * @class Point
     * @brief Representation of a location on a screen or other drawable.
     */
    struct Point {
        ScreenCoordType x{0}, y{0};
        bool set{false};

        constexpr Point() = default;
        constexpr Point(const Point&) = default;
        constexpr Point(Point &&) = default;
        constexpr Point& operator=(const Point &) = default;
        constexpr Point& operator=(Point&&) = default;

        template<typename Tx, typename Ty>
        constexpr Point(Tx X, Ty Y) : x(X), y(Y), set(true) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType>,
                          "Arguments to Point() must be convertable to ScreenCoordType");
        }

        ~Point() = default;

        template<class I>
        requires std::is_integral_v<I>
        constexpr Point& operator=(I i) {
            x = i;
            y = i;
            return *this;
        }

        /**
         * @brief A three-way comparison operator to compare two Point objects.
         * @details The purpose of this operator is to allow us to determine if a Point is contained in a Rectangle.
         * The return value will be std::partial_ordering::equivalent iff both x coordinates and both y coordinates
         * are equal; td::partial_ordering::less at least one coordinate of this point is less than that of o and
         * the other is equal; std::partial_ordering::greater iff both coordinates of this are greater than those of
         * o. Given a Rectangle R with Point A as the top left corner and B as the bottom right corner Point P is
         * contained in R iff A <= P && B > P
         * @param o the other Point
         * @return Point ordering, see Details.
         */
        constexpr std::partial_ordering operator <=> (const Point& o) const {
            auto a = std::strong_order(x, o.x);
            auto b = std::strong_order(y, o.y);

#if 0
            // If x == o.x
            if (std::is_eq(a)) {
                // and y == o.y
                if (std::is_eq(b)) return std::partial_ordering::equivalent;
                // and y < o.y
                if (std::is_lt(b)) return std::partial_ordering::less;
                // and y > o.y
                return std::partial_ordering::unordered;
            } else if (std::is_eq(b)) { // if y == o.y (x != o.x)
                // and x < o.x
                if (std::is_lt(a)) return std::partial_ordering::less;
                // and x > o.x
            } else if (std::is_lt(a) && std::is_lt(b)) // if x < o.x && y < o.y
                return std::partial_ordering::less;
            else if (std::is_gt(a) && std::is_gt(b)) // if x > o.x && y > o.y
                return std::partial_ordering::greater;
            return std::partial_ordering::unordered;
#else
            if (std::is_eq(a) && std::is_eq(b))
                return std::partial_ordering::equivalent;
            else if (std::is_gt(a) && std::is_gt(b))
                return std::partial_ordering::greater;
            else if (std::is_lteq(a) && std::is_lteq(b))
                return std::partial_ordering::less;
            return std::partial_ordering::unordered;
#endif
        }

        constexpr explicit operator bool () const { return set; }

        constexpr static Point CenterScreen(unsigned int screen = 0) {
            auto pos = static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(screen));
            return Point{pos, pos };
        }

        constexpr Point operator+(const Size& size) const {
            Point p{*this}; p.x += size.w; p.y += size.h; return p;
        }

        constexpr Point operator+(int border) const {
            Point p{*this}; p.x += border; p.y += border; return p;
        }

        constexpr Point operator+(const Point &p) const {
            Point r{*this}; r.x += p.x; r.y += p.y; return r;
        }
    };

    struct Padding {
        Point topLeft{};
        Point botRight{};

        constexpr Padding() = default;
        constexpr Padding(const Padding&) = default;
        constexpr Padding(Padding &&) = default;
        constexpr Padding& operator=(const Padding &) = default;
        constexpr Padding& operator=(Padding&&) = default;

        template<typename Tx, typename Ty, typename Tw, typename Th>
        [[maybe_unused]] constexpr Padding(Tx X0, Ty Y0, Tw X1, Th Y1) : topLeft(X0,Y0), botRight(X1,Y1) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType> &&
                          std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                          "Arguments to Padding() must be convertable to ScreenCoordType");
        }

        template<class I>
        requires std::is_integral_v<I>
        constexpr Padding& operator=(I i) {
            topLeft = i;
            botRight = i;
            return *this;
        }

        constexpr Padding operator + (const int i) const {
            Padding result{};
            result.topLeft = topLeft + i;
            result.botRight = botRight + i;
            return result;
        }

        constexpr explicit operator bool () const { return topLeft && botRight; }

        ~Padding() = default;
    };

    struct Rectangle {
        Point point{};
        Size size{};

        constexpr Rectangle() = default;
        constexpr Rectangle(const Rectangle&) = default;
        constexpr Rectangle(Rectangle&&) = default;
        constexpr Rectangle& operator=(const Rectangle &) = default;
        constexpr Rectangle& operator=(Rectangle &&) = default;

        explicit constexpr operator bool () const { return point.set && size.set; }

        template<typename Tx, typename Ty, typename Tw, typename Th>
        [[maybe_unused]] constexpr Rectangle(Tx X, Ty Y, Tw W, Th H) : point(X,Y), size(W,H) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType> &&
                    std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                    "Arguments to Size() must be convertable to ScreenCoordType");
        }

        /**
         * @brief Construct a Rectangle from a Point and a Size.
         * @param point The Point.
         * @param size The Size.
         */
        [[maybe_unused]] constexpr Rectangle(const Point& point, const Size &size) : point(point), size(size) {}

        constexpr Rectangle& operator=(const Point &p) {
            point = p;
            return *this;
        }

        constexpr Rectangle& operator=(const Size &s) {
            size = s;
            return *this;
        }

        constexpr Rectangle operator + (const Point offset) const {
            Rectangle result{*this};
            result.point = result.point + offset;
            return  result;
        }

        [[nodiscard]] constexpr bool contains(const Point& p) const {
            return point <= p && (point + size) > p;
        }

        [[nodiscard]] constexpr Rectangle intersection(const Rectangle &o) const {
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


inline constexpr rose::Size operator + (const rose::Size &s, const rose::Point& p) {
    return rose::Size {s.w + p.x, s.h + p.y};
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

    template<>
    class formatter<rose::Padding> {
        char presentation_ = 'd';
    public:
        // parse format and store it
        constexpr auto parse(format_parse_context &ctx) {
            auto i = ctx.begin(), end = ctx.end();
            if (i != end && (*i == 'd' || *i == 'o')) {
                presentation_ = *i++;
            }
            if (i != end && *i != '}') {
                throw format_error("Invalid Padding format.");
            }
            return i;
        }

        // format a value using stored specification:
        template<class FmtContext>
        constexpr auto format(const rose::Padding &r, FmtContext &ctx) const {
            // note: we can't use ternary operator '?:' in a constexpr
            switch (presentation_) {
                default:
                    // 'ctx.out()' is an output iterator
                case 'd':
                    return format_to(ctx.out(), "[{:d},{:d},{:d},{:d}]", r.topLeft.x, r.topLeft.y, r.botRight.x, r.botRight.y);
            }
        }
    };
}

inline rose::Size operator - (const rose::Point &p0, const rose::Point &p1) {
    return {p0.x - p1.x, p0.y - p1.y};
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
