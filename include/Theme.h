
/*
 * Theme.h Created by Richard Buckley (C) 08/01/23
 */

/**
 * @file Theme.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 08/01/23
 * @brief The Theme class holds data used in visual presentation of the scene. By using data from a theme in all
 * Gadgets an application can present a unified visual experience.
 * @details
 */

#ifndef ROSE2_THEME_H
#define ROSE2_THEME_H

#include <Color.h>
#include <RoseTypes.h>

namespace rose {

    enum class ThemeColor : size_t {
        Base,
        Top,
        Bottom,
        Left,
        Right,
        Invert,
        Text,
        Alert,
        Ok,
        Warning,
        AlertText,
        OkText,
        WaningText,
        ShadeCount,
    };

    template<typename T>
    struct ThemeColorArray : public std::array<T,static_cast<size_t>(ThemeColor::ShadeCount)> {
        using ArrayType = std::array<T,static_cast<size_t>(ThemeColor::ShadeCount)>;

        ThemeColorArray() : ArrayType() {}

        T& operator[](ThemeColor idx) {
            return ArrayType::operator[](static_cast<size_t>(idx));
        }

        constexpr const T& operator[](ThemeColor idx) const {
            return ArrayType::operator[](static_cast<size_t>(idx));
        }
    };

    /**
     * @struct Theme
     */
    struct Theme {

        ThemeColorArray<HSVA> hsvaShades{};
        ThemeColorArray<Color> colorShades{};

        ScreenCoordType borderSize{6};

        Color screenBackground{color::DarkGrey};
        Color gadgetBackground{color::DarkGrey};
        Color activeBackground{color::MediumGrey};
        Color enterBackground{color::DarkGrey};
        Color textForeground{color::LightGrey};

        Corners corners{Corners::SQUARE};
        Visual visual{Visual::SHAPED};

        Theme() = default;
        Theme(const Theme&) = default;
        Theme(Theme&&) = default;
        Theme& operator = (const Theme&) = default;
        Theme& operator = (Theme&&) = default;

        ~Theme() = default;

        [[maybe_unused]] void setThemeShade(const HSVA& shade);

        [[maybe_unused]] void setThemeColors(const HSVA& red, const HSVA& green, const HSVA& yellow);

        [[maybe_unused]] void setThemeTextColors(const HSVA& red, const HSVA& green, const HSVA& yellow);

        [[maybe_unused]] void updateThemeColors();
    };

} // rose

#endif //ROSE2_THEME_H
