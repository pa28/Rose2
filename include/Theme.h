//
// Created by richard on 08/01/23.
//

/*
 * Theme.h Created by Richard Buckley (C) 08/01/23
 */

/**
 * @file Theme.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 08/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_THEME_H
#define ROSE2_THEME_H

#include <Color.h>
#include <RoseTypes.h>

namespace rose {

    /**
     * @struct Theme
     */
    struct Theme {
        constexpr static size_t IMPRINT = 0;
        constexpr static size_t UP_BG = 1;
        constexpr static size_t DN_BG = 2;
        constexpr static size_t UP_SDW = 3;
        constexpr static size_t DN_SDW = 4;
        constexpr static size_t SHADE_COUNT = 5;

        constexpr static std::array<float, SHADE_COUNT> intensities = { 1.0f, 0.393f, 0.286f, 0.571f, 0.214f };

        std::array<Color, SHADE_COUNT> shades{};

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

        [[maybe_unused]] void setThemeShade(const Color& shade);
    };

} // rose

#endif //ROSE2_THEME_H
