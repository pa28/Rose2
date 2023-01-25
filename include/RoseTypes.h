//
// Created by richard on 09/01/23.
//

#ifndef ROSE2_ROSETYPES_H
#define ROSE2_ROSETYPES_H

#include <array>
#include <Signals.h>

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

    /**
     * @class AnimationProtocol
     * @brief This protocol sends a 64 bit value of the number of milliseconds since the library was
     * initialized. This is useful for short animation tasks.
     */
    class AnimationProtocol : public Protocol<uint64_t> {};

    class ButtonStateProtocol : public Protocol<bool, uint64_t> {};

    inline std::array<unsigned char, 8> utf8(unsigned int uc) {
        std::array<unsigned char, 8> seq{};
        size_t n = 0;
        if (uc < 0x80) n = 1;
        else if (uc < 0x800) n = 2;
        else if (uc < 0x10000) n = 3;
        else if (uc < 0x200000) n = 4;
        else if (uc < 0x4000000) n = 5;
        else if (uc <= 0x7fffffff) n = 6;
        seq[n] = static_cast<unsigned char>('\0');
        switch (n) {
            case 6:
                seq[5] = 0x80u | (uc & 0x3fu);
                uc = uc >> 6u;
                uc |= 0x4000000u;
                [[fallthrough]];
            case 5:
                seq[4] = 0x80u | (uc & 0x3fu);
                uc = uc >> 6u;
                uc |= 0x200000u;
                [[fallthrough]];
            case 4:
                seq[3] = 0x80u | (uc & 0x3fu);
                uc = uc >> 6u;
                uc |= 0x10000u;
                [[fallthrough]];
            case 3:
                seq[2] = 0x80u | (uc & 0x3fu);
                uc = uc >> 6u;
                uc |= 0x800u;
                [[fallthrough]];
            case 2:
                seq[1] = 0x80u | (uc & 0x3fu);
                uc = uc >> 6u;
                uc |= 0xc0u;
                [[fallthrough]];
            case 1:
                seq[0] = uc & 0xffu;
                [[fallthrough]];
            default:
                break;
        }
        return seq;
    }

}

#endif //ROSE2_ROSETYPES_H
