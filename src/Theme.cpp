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
    [[maybe_unused]] void Theme::setThemeShade(const HSVA &shade) {
        hsvaShades[ThemeColor::Base] = shade;
        hsvaShades[ThemeColor::Top] = shade.modValue(0.2f);
        hsvaShades[ThemeColor::Bottom] = shade.modValue(-0.15f);
        hsvaShades[ThemeColor::Left] = shade.modValue(0.1f);
        hsvaShades[ThemeColor::Right] = shade.modValue(-0.15f);
        hsvaShades[ThemeColor::Invert] = shade.modValue(-0.075f);
        hsvaShades[ThemeColor::Text] = shade.contrasting();
    }

    void Theme::setThemeColors(const HSVA &red, const HSVA &green, const HSVA &yellow) {
        hsvaShades[ThemeColor::Red] = red;
        hsvaShades[ThemeColor::Green] = green;
        hsvaShades[ThemeColor::Yellow] = yellow;
    }

    void Theme::setThemeTextColors(const HSVA &red, const HSVA &green, const HSVA &yellow) {
        hsvaShades[ThemeColor::RedText] = red;
        hsvaShades[ThemeColor::GreenText] = green;
        hsvaShades[ThemeColor::YellowText] = yellow;
    }

    void Theme::updateThemeColors() {
        std::ranges::transform(hsvaShades.begin(), hsvaShades.end(), colorShades.begin(),
                               [](const HSVA& hsva) -> Color { return hsva.color(); });
        for (const auto &color : colorShades) {
            fmt::print("Color: {}\n", color);
        }
    }

} // rose