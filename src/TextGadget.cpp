//
// Created by richard on 03/01/23.
//

/*
 * TextGadget.cpp Created by Richard Buckley (C) 03/01/23
 */

/**
 * @file TextGadget.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 03/01/23
 */

#include "TextGadget.h"

namespace rose {
    void TextGadget::createTexture(Context &context) {
        if (mText.empty())
            return;

        if (!mFont) {
            FontCache &fontCache = FontCache::getFontCache();
            mFont = fetchFont(fontCache, mFontName, mPointSize);
        }

        mTextSize = Size();
        if (mFont) {
            Surface surface{};
            auto textAndSuffix = mText;
            auto fgColor = mTextFgColor;

            switch (mRenderStyle) {
                case Blended:
                    surface.reset(TTF_RenderUTF8_Blended(mFont.get(), textAndSuffix.c_str(), fgColor.sdlColor()));
                    break;
                case Shaded:
                    surface.reset(
                            TTF_RenderUTF8_Shaded(mFont.get(), textAndSuffix.c_str(), fgColor.sdlColor(),
                                                  mVisualMetrics.background.sdlColor()));
                    break;
                case Solid:
                    surface.reset(TTF_RenderUTF8_Solid(mFont.get(), textAndSuffix.c_str(), fgColor.sdlColor()));
                    break;
            }
            if (surface) {
                mTextSize = Size{surface->w, surface->h};
                mTexture.reset(SDL_CreateTextureFromSurface(context.get(), surface.get()));
                if (!mTexture)
                    throw TextGadgetException( fmt::format("Texture error: {}", SDL_GetError()));
            } else {
                throw TextGadgetException( fmt::format("Surface error: {}", SDL_GetError()));
            }
        } else {
            throw TextGadgetException( fmt::format("Font error"));
        }
    }

    void TextGadget::textUpdated() {
        mTextRenderRequired = true;
        mTexture.reset();
    }

    bool TextGadget::initialGadgetLayout(Context &context) {
        if (!mVisualMetrics.gadgetPadding)
            mVisualMetrics.gadgetPadding = 5;
        if (!mText.empty()) {
            try {
                createTexture(context);
                mVisualMetrics.desiredSize = mTextSize;
            } catch (TextGadgetException &e) {
                fmt::print("{}\n", e.what());
            }
        }
        return Gadget::initialGadgetLayout(context);
    }

    void TextGadget::draw(Context &context, Point drawLocation) {
        Gadget::draw(context, drawLocation);
        if (mTexture) {
            Rectangle textRenderRect = mVisualMetrics.renderRect + drawLocation;
            context.renderCopy(mTexture, textRenderRect);
        }
    }

} // rose