//
// Created by richard on 08/01/23.
//

/*
 * Theme.cpp Created by Richard Buckley (C) 08/01/23
 */

/**
 * @file Theme.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 08/01/23
 */

#include "Theme.h"

namespace rose {
    [[maybe_unused]] void Theme::setThemeShade(const Color &shade) {
        for (auto idx = 0u; idx < SHADE_COUNT; ++idx) {
            shades[idx] = shade.attenuate(intensities[idx]);
        }
    }
} // rose