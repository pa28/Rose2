/**
 * @file TextGadget.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 03/01/23
 */

#include "TextGadget.h"
#include <Window.h>
#include <Application.h>

namespace rose {
    std::unique_ptr<Material> IconGadget::mMaterial{};

    std::unique_ptr<FontCache> TextGadget::mFontCache{};

    TextGadget::TextGadget(std::shared_ptr<Theme> &theme) : Gadget(theme) {
        mTextFgColor = theme->colorShades[ThemeColor::Text];
        mRenderStyle = theme->textRenderStyle;
        mFontName = theme->fontName;
        mPointSize = theme->textPointSize;
        mVisualMetrics.gadgetPadding = theme->textPadding;
    }

    void TextGadget::createTexture(Context &context) {
        if (mText.empty())
            return;

        if (!mFont) {
            mFont = getFont(mFontName, mPointSize);
        }

        mTextSize = Size();
        if (mFont) {
            Surface surface{};
            auto textAndSuffix = mText;
            auto fgColor = mTextFgColor;

            switch (mRenderStyle) {
                case RenderStyle::Blended:
                    surface.reset(TTF_RenderUTF8_Blended(mFont.get(), textAndSuffix.c_str(), fgColor.sdlColor()));
                    break;
                case RenderStyle::Shaded:
                    surface.reset(
                            TTF_RenderUTF8_Shaded(mFont.get(), textAndSuffix.c_str(), fgColor.sdlColor(),
                                                  mVisualMetrics.background.sdlColor()));
                    break;
                case RenderStyle::Solid:
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
        setNeedsDrawing();
        setNeedsLayout();
        mTextRenderRequired = true;
        mTexture.reset();
    }

    bool TextGadget::initialLayout(Context &context) {
        if (!mText.empty()) {
            try {
               createTexture(context);
                mVisualMetrics.desiredSize = mTextSize;
            } catch (TextGadgetException &e) {
                fmt::print("{}\n", e.what());
            }
        }
        auto result = Gadget::initialLayout(context);

        return result;
    }

    void TextGadget::draw(Context &context, Point drawLocation) {
        Gadget::draw(context, drawLocation);
        if (mTexture) {
            Rectangle textRenderRect = mVisualMetrics.renderRect + drawLocation;
            context.renderCopy(mTexture, textRenderRect);
        }
    }

    IconGadget::IconGadget(std::shared_ptr<Theme> &theme) : TextGadget(theme) {
        mFontName = theme->iconFontName;
        mPointSize = theme->iconPointSize;
        mAnimationSlot->receiver = [](uint64_t ticks) {
            fmt::print("{} - ticks: {}\n", "IconGadget", ticks);
        };
    }

#if 1
    void IconGadget::createIconTexture(Context &context) {
        if (!mFont) {
            mFont = mMaterial->getFont(mPointSize);
        }

        mTextSize = Size();

        auto utf8Data = utf8(mIconCode);
        Surface surface{TTF_RenderUTF8_Blended(mFont.get(), reinterpret_cast<const char *>(utf8Data.data()),
                                               mTextFgColor.sdlColor())};

        int minX = surface->w;
        int minY = surface->h;
        int maxX = 0, maxY = 0;

        for (auto y = 0; y < surface->h; ++y) {
            for (auto x = 0; x < surface->w; ++x) {
                auto rgba = Color::toColor(surface->format, surface.pixel(x, y));
                if (rgba[Color::ALPHA] > 0.f) {
                    minX = std::min(minX, x);
                    minY = std::min(minY, y);
                    maxX = std::max(maxX, x);
                    maxY = std::max(maxY, y);
                }
            }
        }

#ifdef MINIMIZE_ENTYPO
        Surface minimal{maxX - minX + 1, maxY - minY + 1};
        for (auto y = 0; y < minimal->h; ++y) {
            for (auto x = 0; x < minimal->w; ++x) {
                auto rgba = Color::toColor(surface->format, surface.pixel(minX + x, minY + y));
                minimal.pixel(x, y) = Color::mapSdl(minimal->format, rgba);
            }
        }
        mTexture = minimal.toTexture(context);
#else
        mTexture = surface.toTexture(context);
#endif
        mTextSize = mTexture.getSize();
    }

    bool IconGadget::initialLayout(Context &context) {
        if (mIconCode) {
            try {
                createIconTexture(context);
                mVisualMetrics.desiredSize = mTextSize;
            } catch (TextGadgetException &e) {
                fmt::print("{}\n", e.what());
            }
        }
        auto result = TextGadget::initialLayout(context);

        return result;
    }

    void IconGadget::draw(Context &context, Point drawLocation) {
        TextGadget::draw(context, drawLocation);
    }

    void IconGadget::initialize() {
        if (mIsInitialized)
            return;

        TextGadget::initialize();
    }

#endif

} // rose