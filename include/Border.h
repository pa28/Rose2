//
// Created by richard on 09/01/23.
//

/*
 * Border.h Created by Richard Buckley (C) 09/01/23
 */

/**
 * @file Border.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 09/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_BORDER_H
#define ROSE2_BORDER_H

#include <RoseTypes.h>
#include <GraphicsModel.h>

namespace rose {
    class Gadget;

    /**
     * @class Border
     */
    class Border : public Texture {
    protected:
        bool mRendered{false};
        Corners mCorners{Corners::SQUARE};
        Color mBackground{color::OpaqueBlack};
        Visual mVisual{Visual::FLAT};

    public:
        Border() = default;
        Border(const Border &) = delete;
        Border(Border &&) = default;
        Border& operator = (const Border&) = delete;
        Border& operator = (Border &&) = default;

        /**
         * @brief Create a Border
         * @details Builds a Texture compatible with building up textures within Rose . The pixel format is
         * SDL_PIXELFORMAT_RGBA8888, the texture access is SDL_TEXTUREACCESS_TARGET.
         * @param context The renderer to use.
         * @param size The size of the texture.
         */
        [[maybe_unused]] Border(Context &context, const Size& size, const Theme& theme) : Texture(context, size) {
            createThemedBorder(context, theme);
        }

        void createThemedBorder(Context &context, const Theme& theme);

        void layout(rose::Context &context, Gadget &gadget);

        void renderBorder(Context &context, Gadget &gadget);

        void draw(rose::Context &context, const rose::Point &drawLocation, Gadget &gadget);

        ~Border() = default;
    };
}


#endif //ROSE2_BORDER_H
