//
// Created by richard on 09/01/23.
//

#ifndef ROSE2_ROSETYPES_H
#define ROSE2_ROSETYPES_H

namespace rose {
    typedef int ScreenCoordType;   ///< The type used to represent a screen coordinate.

    /**
     * The type of corners a supporting Gadget should render.
     */
    enum class Corners {
        UNSET,              ///< Not set, use Theme value.
        SQUARE,             ///< Square corners, provides a rectangular shape.
        ROUND,              ///< Rounded corners.
    };

    enum class Visual {
        UNSET,              ///< Not set, use Theme value.
        FLAT,               ///< Flat, no 3D simulation. Actions implemented by changing background color.
        SHADOW,             ///< Simulated 3D. Actions implemented by flipping Texture horizontally and vertically.
    };

    enum class Actions {
        ENTER, LEAVE, ACTIVATED, DEACTIVATED,
    };
}

#endif //ROSE2_ROSETYPES_H
