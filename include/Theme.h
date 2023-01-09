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
        ScreenCoordType borderSize{6};

        Color screenBackground{color::DarkGrey};
        Color gadgetBackground{color::MediumGrey};
        Color textForeground{color::LightGrey};

        Corners corners{Corners::SQUARE};

        Theme() = default;
        Theme(const Theme&) = default;
        Theme(Theme&&) = default;
        Theme& operator = (const Theme&) = default;
        Theme& operator = (Theme&&) = default;

        ~Theme() = default;
    };

} // rose

#endif //ROSE2_THEME_H
