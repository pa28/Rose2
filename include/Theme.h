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

namespace rose {

/**
 * @class Theme
 */
    struct Theme {
        Color screenBackground{color::OpaqueBlack};
    };

} // rose

#endif //ROSE2_THEME_H
