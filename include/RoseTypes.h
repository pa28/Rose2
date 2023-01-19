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
        SQUARE,             ///< Square corners, provides a rectangular shape.
        ROUND,              ///< Rounded corners.
    };

    enum class Visual {
        FLAT,               ///< Flat, no 3D simulation. Actions implemented by changing background color.
        SHADOW,             ///< Simulated 3D Shadow box. Actions implemented by flipping Texture horizontally and vertically.
        NOTCH,              ///< Simulated 3D Notch.
        RIDGE,              ///< Simulated 3D Ridge.
    };

    enum class RenderStyle{
        Blended,        ///< Render text blended on a transparent background.
        Shaded,         ///< Render text shaded on a solid background.
        Solid           ///< Render text without blending or shading. Fastest but lowest quality.
    };


    enum class Actions {
        ENTER, LEAVE, ACTIVATED, DEACTIVATED,
    };
}

#endif //ROSE2_ROSETYPES_H
