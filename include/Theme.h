
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
        Base,           ///< The base shade of the Theme color set, also the default background.
        Top,            ///< Top shade for 3D representations.
        Bottom,         ///< Bottom shade for 3D representations.
        Left,           ///< Left shade for 3D representations.
        Right,          ///< Right shade for 3D representations.
        Invert,         ///< Inversion of the base shade
        Text,           ///< Default text and general foreground rendering color.
        Alert,          ///< A color to denote an Alert state.
        Ok,             ///< A color to denote an OK state.
        Warning,        ///< A color to denote a Warning state.
        AlertText,      ///< A color for Alert state text.
        OkText,         ///< A color for OK state text.
        WaningText,     ///< A color for Warning state text.
        ShadeCount,     ///< The number of shades in a ThemeColorArray.
    };

    /**
     * @struct ThemeColorArray
     * @brief A std::array to store the color shades used in the Theme.
     * @tparam ColorType the type of color stored in the array.
     */
    template<typename ColorType>
    struct ThemeColorArray : public std::array<ColorType,static_cast<size_t>(ThemeColor::ShadeCount)> {
        /**
         * A convenience type definition for the full std::array type.
         */
        using ArrayType = std::array<ColorType,static_cast<size_t>(ThemeColor::ShadeCount)>;

        ThemeColorArray() : ArrayType() {}

        ColorType& operator[](ThemeColor idx) {
            return ArrayType::operator[](static_cast<size_t>(idx));
        }

        constexpr const ColorType& operator[](ThemeColor idx) const {
            return ArrayType::operator[](static_cast<size_t>(idx));
        }
    };

    /**
     * @struct Theme
     */
    struct Theme {

        ThemeColorArray<HSVA> hsvaShades{};                 ///< HSV shades, used to manipulate colors.
        ThemeColorArray<Color> colorShades{};               ///< RGV shades, used to generate SDL_Colors for drawing.

        ScreenCoordType borderSize{6};                      ///< The default border size for the Theme.

        Corners corners{Corners::SQUARE};                   ///< The type of corners used.
        Visual visual{Visual::SHADOW};                      ///< The type of visual used.

        Theme() = default;
        Theme(const Theme&) = default;
        Theme(Theme&&) = default;
        Theme& operator = (const Theme&) = default;
        Theme& operator = (Theme&&) = default;

        ~Theme() = default;

        /**
         * @brief Generate a set of Theme shades from the provided Base shade.
         * @details The HSV shades [Base..Text] are derived from the Base shade. Must be followed by a call to
         * updateThemeColors to convert the HSV colors to RGB.
         * @param shade The base HSV shade.
         */
        [[maybe_unused]] void setThemeShade(const HSVA& shade);

        /**
         * @brief Set the shades to use for Alert, OK, and Warning conditions.
         * @details This method sets the HSV shades [Alert..Warning]. Must be followed by a call to
         * updateThemeColors to convert the HSV colors to RGB.
         * @param alert the Alert shade.
         * @param ok the OK shade.
         * @param warning the Warning shade.
         */
        [[maybe_unused]] void setThemeColors(const HSVA& alert, const HSVA& ok, const HSVA& warning);

        /**
         * @brief Set the shades to use for text in Alert, OK, and Warning conditions.
         * @details This method sets the HSV shades [AlertText..WarningText]. Must be followed by a call to
         * updateThemeColors to convert the HSV colors to RGB.
         * @param alertText the Alert text shade.
         * @param okText the OK text shade.
         * @param warningText the Warning text shade.
         */
        [[maybe_unused]] void setThemeTextColors(const HSVA& alertText, const HSVA& okText, const HSVA& warningText);

        /**
         * @brief Update the RGB shade set from the HSV shade set.
         * @details This method iterates over hsvaShades converting them to RGB to store in colorShades.
         */
        [[maybe_unused]] void updateThemeColors();
    };

} // rose

#endif //ROSE2_THEME_H
