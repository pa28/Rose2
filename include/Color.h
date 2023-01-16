
/*
 * Color.h Created by Richard Buckley (C) 02/01/23
 */

/**
 * @file Theme.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 02/01/23
 * @brief Color management classes and functions.
 * @details Colors are managed by using ColorArray, Color, HSLA, and HSVA.<br/>
 * ColorArray is the base of the other three. Derived from std::array it provides the storage for the four channels
 * required to specify a color to draw: three to specify the color and one for the alpha (transparency) channel.<br/>
 * Color provides basic RGBA color specification as wells as conversion to HSLA, HSVA and ultimately SDL library
 * colors.<br/>
 * HSLA is a Hue, Saturation, Lightness color implementation.</br>
 * HSVA is a Hue, Saturation, Value color implementations. HSVA is used within the library for high level color
 * manipulation within a set of colors designed to provide a unified visual appearance.<br/>
 */

#ifndef ROSE2_COLOR_H
#define ROSE2_COLOR_H

#include <SDL.h>
#include <array>
#include <algorithm>
#include <cmath>
#include <fmt/format.h>


namespace rose {

    /**
     * @brief The base class for all four component color values stored as floats.
     */
    struct ColorArray : public std::array<float,4> {
        bool set{false};
        explicit operator bool () const noexcept { return set; }

        constexpr ColorArray() : std::array<float,4>() {}

        template<class Ta, class Tb, class Tc, class Td>
        requires std::is_floating_point_v<Ta> && std::is_floating_point_v<Tb> && std::is_floating_point_v<Tc> && std::is_floating_point_v<Td>
        [[maybe_unused]] constexpr ColorArray(Ta a, Tb b, Tc c, Td d) : std::array<float, 4>({a,b,c,d}) {}

        auto operator <=> (const ColorArray &) const = default;
    };

    /**
     * @brief A free function to be able to compute modulus in constexpr context.
     * @tparam T The type of the parameters, must be the same type.
     * @param x The value.
     * @param y The modulus
     * @return The remainder of x / y
     */
    template<typename T>
    static constexpr T mod(T x, T y) noexcept {
        if (y != 0) {
            if constexpr (std::is_integral_v<T>) {
                return x % y;
            } else {
                auto trunc = (float) ((int) (x / y));
                auto r = x - trunc * y;
                return r;
            }
        } else {
            return 0;
        }
    }

    class Color;
    class HSLA;

    /**
     * @class HSVA
     * @brief Representation of a Hue, Saturation, Value color with an Alpha channel.
     */
    class HSVA : public ColorArray {
    private:
        constexpr static size_t HUE = 0;
        constexpr static size_t SATURATION = 1;
        constexpr static size_t VALUE = 2;
        constexpr static size_t ALPHA = 3;

    public:
        HSVA() = default;
        HSVA(const HSVA&) = default;
        HSVA(HSVA&&) = default;
        HSVA& operator=(const HSVA&) = default;
        HSVA& operator=(HSVA&&) = default;

        ~HSVA() = default;

        /**
         * @brief Construct a Hue, Saturation, Value color.
         * @tparam T The type of the parameters, required to be floating point type.
         * @param h Hue value [0..360]
         * @param s Saturation [0..1]
         * @param v Value [0..1]
         * @param a Alpha [0..1]
         */
        template<class T>
        requires std::is_floating_point_v<T>
        constexpr HSVA(T h, T s, T v, T a) : ColorArray({h,s,v,a}) {
            set = true;
        }

        /**
         * @brief Construct a Hue, Saturation Value color.
         * @tparam T he type of the parameters, required to be integral type.
         * @param h Hue [0..360]
         * @param s Saturation [0..100]
         * @param v Value [0..100]
         * @param a Alpha [0..255]
         */
        template<class T>
        requires std::is_integral_v<T>
        constexpr HSVA(T h, T s, T v, T a) : ColorArray() {
            set = true;
            (*this)[HUE] = static_cast<float>(h);
            (*this)[SATURATION] = static_cast<float>(s) / 100.0f;
            (*this)[VALUE] = static_cast<float>(v) / 100.0f;
            (*this)[ALPHA] = static_cast<float>(a) / 255.0f;
        }

        /**
         * @brief Convert to Color
         * @details See <a href="https://www.rapidtables.com/convert/color/hsl-to-rgb.html">hsl to rgb</a>
         * @return RGBA colour Value.
         */
        [[nodiscard]] constexpr Color color() const noexcept;

        /**
         * @brief Modify the Value channel (brightness) of an HSV color.
         * @param dValue The delta to apply to the value.
         * @return A new HSVA.
         */
        [[nodiscard]] constexpr HSVA modValue(float dValue) const noexcept {
            HSVA result{*this};
            result[VALUE] = std::clamp(result[VALUE] + dValue, 0.f, 1.f);
            return result;
        }

        /**
         * @brief Get a new contrasting colour to this HSL colour.
         * @return A new contrasting HSL colour.
         */
        [[nodiscard]] constexpr HSVA contrasting() const {
            float value;
            if ((*this)[VALUE] < 0.5f)
                value = (*this)[VALUE] + 0.5f;
            else
                value = (*this)[VALUE] - 0.5f;
            return HSVA{(*this)[HUE], (*this)[SATURATION], value, (*this)[ALPHA]};
        }
    };

    /**
     * @class HSLA
     * @brief Representation of a Hue, Saturation, Lightness color with an Alpha channel.
     */
    class HSLA : public ColorArray {
    private:
        constexpr static size_t HUE = 0;
        constexpr static size_t SATURATION = 1;
        constexpr static size_t LIGHTNESS = 2;
        constexpr static size_t ALPHA = 3;

    protected:

        [[maybe_unused]] explicit constexpr HSLA(const ColorArray& array) : ColorArray(array) {}

    public:
        constexpr HSLA() = default;
        constexpr HSLA(const HSLA &) = default;
        constexpr HSLA(HSLA &&) = default;
        HSLA &operator=(const HSLA &) = default;
        HSLA &operator=(HSLA &&) = default;

        ~HSLA() = default;

        /**
         * @brief Construct a Hue, Saturation, Lightness color.
         * @tparam T The type of the parameters, required to be floating point type.
         * @param h Hue value [0..360]
         * @param s Saturation [0..1]
         * @param v Lightness [0..1]
         * @param a Alpha [0..1]
         */
        template<class T>
        requires std::is_floating_point_v<T>
        constexpr HSLA(T h, T s, T l, T a) : ColorArray({h, s, l, a}) {
            set = true;
        }

        /**
         * @brief Construct a Hue, Saturation, Lightness color.
         * @tparam T The type of the parameters, required to be integral type.
         * @param h Hue value [0..360]
         * @param s Saturation [0..100]
         * @param l Lightness [0..100]
         * @param a Alpha [0..255]
         */
        template<class T>
        requires std::is_integral_v<T>
        constexpr HSLA(T h, T s, T l, T a) : ColorArray() {
            set = true;
            (*this)[HUE] = static_cast<float>(h);
            (*this)[SATURATION] = static_cast<float>(s) / 100.0f;
            (*this)[LIGHTNESS] = static_cast<float>(l) / 100.0f;
            (*this)[ALPHA] = static_cast<float>(a) / 255.0f;
        }

        /**
         * @brief Modify the Lightness channel (amount of white) of an HSLA color.
         * @param dValue The delta to apply to the value.
         * @return A new HSLA.
         */
        [[maybe_unused]] [[nodiscard]] constexpr HSLA modValue(float dValue) const noexcept {
            HSLA result{*this};
            result[LIGHTNESS] = std::clamp(result[LIGHTNESS] + dValue, 0.f, 1.f);
            return result;
        }

        /**
         * @brief Convert to Color
         * @details See <a href="https://www.rapidtables.com/convert/color/hsl-to-rgb.html">hsl to rgb</a>
         * @return RGBA colour Value.
         */
        [[nodiscard]] constexpr Color color() const noexcept;
    };

    /**
     * @class Color
     * Representation of a Red, Green, Blue color with an Alpha channel.
     */
    class Color : public ColorArray {
    private:
        constexpr static size_t RED = 0;
        constexpr static size_t GREEN = 1;
        constexpr static size_t BLUE = 2;
        constexpr static size_t ALPHA = 3;

    public:
        Color() = default;
        Color(const Color&) = default;
        Color(Color&&) = default;
        Color& operator=(const Color&) = default;
        Color& operator=(Color&&) = default;
        ~Color() = default;

        /**
         * @brief Construct a Red, Green, Blue color.
         * @tparam T The type of the parameters, required to be floating point type.
         * @param r Red [0..1]
         * @param g Green [0..1]
         * @param b Blue [0..1]
         * @param a Alpha [0..1]
         */
        template<class T>
        requires std::is_floating_point_v<T>
        constexpr Color(T r, T g, T b, T a) : ColorArray() {
            set = true;
            (*this)[RED] = static_cast<float>(r);
            (*this)[GREEN] = static_cast<float>(g);
            (*this)[BLUE] = static_cast<float>(b);
            (*this)[ALPHA] = static_cast<float>(a);
            std::ranges::transform( begin(), begin(), begin(),
                                    [] (auto chan) { return fmin(1.0, fabs(chan)); });
        }

        /**
         * @brief Construct a Hue, Saturation, Lightness color.
         * @tparam T The type of the parameters, required to be integral type.
         * @param r Red [0..255]
         * @param g Green [0..255]
         * @param b Blue [0..255]
         * @param a Alpha [0..255]
         */
        template<class T>
        requires std::is_integral_v<T>
        constexpr Color(T r, T g, T b, T a) : ColorArray() {
            set = true;
            (*this)[RED] = static_cast<float>(r) / 255.0f;
            (*this)[GREEN] = static_cast<float>(g) / 255.0f;
            (*this)[BLUE] = static_cast<float>(b) / 255.0f;
            (*this)[ALPHA] = static_cast<float>(a) / 255.0f;
        }

        /**
         * @brief Convert an RGB color to an SDL_Color.
         * @return SDL_Color.
         */
        [[maybe_unused]] [[nodiscard]] constexpr SDL_Color sdlColor() const {
            std::array<Uint8,4> sdlChan{};
            std::ranges::transform(begin(), end(), sdlChan.begin(),
                                   [] (auto chan) {
                return static_cast<Uint8>(255. * fabs(chan));
            });
            return SDL_Color{sdlChan[RED], sdlChan[GREEN], sdlChan[BLUE], sdlChan[ALPHA]};
        }

        auto operator <=> (const Color &) const = default;

        /**
         * @brief Attenuate the intensity by multiplying Red, Green and Blue by intensity.
         * @param intensity The intensity modulation value.
         * @return The generated Color.
         */
        [[maybe_unused]] [[nodiscard]] Color attenuate(float intensity) const;

        /**
         * @brief Convert to HSVA
         * @return The generated HSVA
         */
        [[maybe_unused]] [[nodiscard]] constexpr HSVA hsva() const;

        /**
         * @brief Convert to HSLA
         * @return The generated HSLA
         */
        [[maybe_unused]] [[nodiscard]] constexpr HSLA hsla() const;
    };

    [[maybe_unused]] constexpr HSVA Color::hsva() const {
        auto cMax = std::max((*this)[RED], std::max((*this)[GREEN], (*this)[BLUE]));
        auto cMin = std::min((*this)[RED], std::min((*this)[GREEN], (*this)[BLUE]));
        auto delta = cMax - cMin;

        float hue = 0.f, saturation = 0.f;

        if (delta == 0.f) {
            hue = 0.f;
        } else if (cMax == (*this)[RED]) {
            hue = 60.f * fmod(((*this)[GREEN]-(*this)[BLUE]) / delta, 6.f);
        } else if (cMax == (*this)[GREEN]) {
            hue = 60.f * (((*this)[BLUE]-(*this)[RED])/delta + 2.f);
        } else if (cMax == (*this)[BLUE]) {
            hue = 60.f * (((*this)[RED]-(*this)[GREEN])/delta + 4.f);
        }

        if (delta != 0.f) {
            saturation = delta / cMax;
        }

        auto value = cMax;

        return HSVA{hue, saturation, value, (*this)[ALPHA]};
    }

    [[maybe_unused]] constexpr HSLA Color::hsla() const {
        auto cMax = std::max((*this)[RED], std::max((*this)[GREEN], (*this)[BLUE]));
        auto cMin = std::min((*this)[RED], std::min((*this)[GREEN], (*this)[BLUE]));
        auto delta = cMax - cMin;

        float hue = 0.f, saturation = 0.f;

        if (delta == 0.f) {
            hue = 0.f;
        } else if (cMax == (*this)[RED]) {
            hue = 60.f * mod(((*this)[GREEN]-(*this)[BLUE]) / delta, 6.f);
        } else if (cMax == (*this)[GREEN]) {
            hue = 60.f * (((*this)[BLUE]-(*this)[RED])/delta + 2.f);
        } else if (cMax == (*this)[BLUE]) {
            hue = 60.f * (((*this)[RED]-(*this)[GREEN])/delta + 4.f);
        }

        auto level = (cMax + cMin) / 2.f;

        if (delta != 0.f) {
            saturation = delta / (1.f - fabs(2.f * level - 1.f));
        }

        return HSLA{hue, saturation, level, (*this)[ALPHA]};
    }

    constexpr Color HSVA::color() const noexcept {
        auto hue = (*this)[HUE];
        float saturation = (*this)[SATURATION];
        float value = (*this)[VALUE];
        float C = value * saturation;
        float X = C * (1 - abs(mod(hue / 60.0f, 2.f) - 1.f));
        float m = value - C;
        float r, g, b;
        if (hue >= 0 && hue < 60) {
            r = C, g = X, b = 0;
        } else if (hue >= 60 && hue < 120) {
            r = X, g = C, b = 0;
        } else if (hue >= 120 && hue < 180) {
            r = 0, g = C, b = X;
        } else if (hue >= 180 && hue < 240) {
            r = 0, g = X, b = C;
        } else if (hue >= 240 && hue < 300) {
            r = X, g = 0, b = C;
        } else {
            r = C, g = 0, b = X;
        }

        return Color{r + m, g + m, b + m, (*this)[ALPHA]};
    }

    constexpr Color HSLA::color() const noexcept {
        auto hue = (*this)[HUE];
        float saturation = (*this)[SATURATION];
        float lightness = (*this)[LIGHTNESS];
        float C = (1.f - fabs(2.f * lightness - 1.f)) * saturation;
        float X = C * (1 - abs(mod(hue / 60.0f, 2.f) - 1.f));
        float m = lightness - C / 2.f;
        float r, g, b;
        if (hue >= 0 && hue < 60) {
            r = C, g = X, b = 0;
        } else if (hue >= 60 && hue < 120) {
            r = X, g = C, b = 0;
        } else if (hue >= 120 && hue < 180) {
            r = 0, g = C, b = X;
        } else if (hue >= 180 && hue < 240) {
            r = 0, g = X, b = C;
        } else if (hue >= 240 && hue < 300) {
            r = X, g = 0, b = C;
        } else {
            r = C, g = 0, b = X;
        }

        return Color {r + m, g + m, b + m, (*this)[ALPHA]};
    }

    namespace color {
        [[maybe_unused]] constexpr Color TransparentBlack{0.0, 0.0, 0.0, 0.0};
        [[maybe_unused]] constexpr Color OpaqueBlack{0.0, 0.0, 0.0, 1.0};
        [[maybe_unused]] constexpr Color OpaqueWhite{1.0, 1.0, 1.0, 1.0};
        [[maybe_unused]] constexpr Color LightGrey{ 0.7, 0.7, 0.7, 1.0 };
        [[maybe_unused]] constexpr Color MediumGrey{ 0.45, 0.45, 0.45, 1.0 };
        [[maybe_unused]] constexpr Color DarkGrey{ 0.2, 0.2, 0.2, 1.0 };

        // https://www.color-hex.com/color-palette/105943

        [[maybe_unused]] constexpr HSVA DarkRed{ 0.f, 1.f, 0.6f, 1.f };
        [[maybe_unused]] constexpr HSVA DarkGreen{ 79.f, 1.f, .6f,1.f};
        [[maybe_unused]] constexpr HSVA DarkYellow{ 50.f, 1.f, 0.6f, 1.f };
    }

} // rose

namespace fmt {
    /**
     * @brief A format library formatter for the ColorArray type.
     */
    template<class C>
    requires std::derived_from<C,rose::ColorArray>
    class formatter<C> {
        char presentation_ = 'f';
        std::string_view prefix_{};

    public:
        // parse format and store it
        constexpr auto parse(format_parse_context &ctx) {
            if constexpr (std::is_same_v<C,rose::Color>)
                prefix_ = "RGBA";
            else if constexpr (std::is_same_v<C,rose::HSVA>)
                prefix_ = "HSVA";
            else if constexpr (std::is_same_v<C,rose::HSLA>)
                prefix_ = "HSLA";
            else
                prefix_ = "UNKN";
            auto i = ctx.begin(), end = ctx.end();
            if (i != end && (*i == 'f' || *i == 'e')) {
                presentation_ = *i++;
            }
            if (i != end && *i != '}') {
                throw format_error("Invalid Color format.");
            }
            return i;
        }

        // format a value using stored specification:
        template<class FmtContext>
        constexpr auto format(const rose::ColorArray &p, FmtContext &ctx) const {
            // note: we can't use ternary operator '?:' in a constexpr
            switch (presentation_) {
                default:
                    // 'ctx.out()' is an output iterator
                case 'f':
                    return format_to(ctx.out(), "{}[{:f},{:f},{:f},{:f}]",
                                     prefix_, p[0], p[1], p[2], p[3]);
                case 'e':
                    return format_to(ctx.out(), "{}[{:e},{:e},{:e},{:e}]",
                                     prefix_, p[0], p[1], p[2], p[3]);
            }
        }
    };
}

#endif //ROSE2_COLOR_H
