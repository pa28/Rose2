//
// Created by richard on 02/01/23.
//

/*
 * Color.cpp Created by Richard Buckley (C) 02/01/23
 */

#include "Color.h"

namespace rose {

    Color Color::attenuate(float intensity) const {
        Color result{};
        for (auto idx = RED; idx < ALPHA; ++idx)
            result.channels[idx] = channels[idx] * intensity;
        result.channels[ALPHA] = channels[ALPHA];
        result.set = true;
        return result;
    }

} // rose
