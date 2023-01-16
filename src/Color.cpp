
/*
 * Color.cpp Created by Richard Buckley (C) 02/01/23
 */

/**
 * @file Color.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 02/01/23
 */

#include "Color.h"

namespace rose {

    [[maybe_unused]] Color Color::attenuate(float intensity) const {
        Color result{};
        for (auto idx = RED; idx < ALPHA; ++idx)
            result[idx] = (*this)[idx] * intensity;
        result[ALPHA] = (*this)[ALPHA];
        result.set = true;
        return result;
    }

} // rose
