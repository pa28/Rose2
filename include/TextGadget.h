//
// Created by richard on 03/01/23.
//

/*
 * TextGadget.h Created by Richard Buckley (C) 03/01/23
 */

/**
 * @file TextGadget.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 03/01/23
 * @brief A Gadget to render a simple block of text.
 * @details
 */

#ifndef ROSE2_TEXTGADGET_H
#define ROSE2_TEXTGADGET_H

#include <Rose.h>
#include <Gadget.h>
#include <Color.h>
#include <Font.h>
#include <Surface.h>
#include <exception>

namespace rose {

    class TextGadgetException : public std::runtime_error {
    public:
        explicit TextGadgetException(const std::string &what_arg) : std::runtime_error(what_arg) {}

        explicit TextGadgetException(const char *what_arg) : std::runtime_error(what_arg) {}
    };

    /**
     * @class TextGadget
     */
    class [[maybe_unused]] TextGadget : public Gadget {
        friend class TextGadgetBuilder;

    public:
        enum RenderStyle{
            Blended,        ///< Render text blended on a transparent background.
            Shaded,         ///< Render text shaded on a solid background.
            Solid           ///< Render text without blending or shading. Fastest but lowest quality.
        };

    protected:
        bool mTextRenderRequired{true};      ///< True when re-rendering of text is required
        std::string mText{};                 ///< The string to render.
        Color mTextFgColor{};                ///< The foreground color to use.
        RenderStyle mRenderStyle{Blended};   ///< The style of rendering Solid, Shaded, or Blended.
        std::string mFontName{};             ///< The name of the True Type Font to use.
        std::shared_ptr<_TTF_Font> mFont{};  ///< The cached font used.
        int mPointSize{10};                  ///< The point (pixel) size of the font.
        Texture mTexture{};                  ///< The generated Texture.
        Size mTextSize{};                    ///< The size of the Texture in pixels.

    public:
        TextGadget() = default;
        TextGadget(const TextGadget&) = delete;
        TextGadget(TextGadget&&) = default;
        TextGadget& operator = (const TextGadget&) = delete;
        TextGadget& operator = (TextGadget&&) = default;
        ~TextGadget() override = default;

        Point layout(Context &context, Rectangle constraint) override;

        void draw(Context &context) override;

        /**
         * @brief Create a Blended Texture from text.
         * @details Fetches the Font corresponding to mFontName and mPointSize, then renders the text in mText as
         * UTF8 to mTexture. The foreground color is set to mTextFgColor. If mRenderStyle is set to Shaded the
         * background color is set to mTextBgColor. The size of the Texture is placed in mTextSize.<p/>
         * If the requested font is not found, or mText is empty any mTexture is reset and mTextSize is set to Zero.
         * @param context The graphics Context.
         * @throws TextGadgetException
         */
        void createTexture(Context &context);

        void textUpdated();

        void setPointSize(int pointSize) {
            if (mPointSize != pointSize) {
                mPointSize = pointSize;
                textUpdated();
            }
        }

        void setForeground(Color color) {
            if (mTextFgColor != color) {
                mTextFgColor = color;
                textUpdated();
            }
        }

        template<class S>
        requires StringLike<S>
        void setFontName(const S &fontName) {
            if (mFontName != fontName) {
                mFontName = fontName;
                textUpdated();
            }
        }

        template<class S>
        requires StringLike<S>
        void setText(const S &text) {
            if (mText != text) {
                mText = text;
                textUpdated();
            }
        }
    };

    /**
     * @class TextGadgetBuilder
     */
    class [[maybe_unused]] TextGadgetBuilder : public GadgetBuilder {
    public:

        TextGadgetBuilder() : GadgetBuilder(std::make_shared<TextGadget>()) {}

        template<class S>
        requires StringLike<S>
        auto text(S content) {
            if (auto gPtr = std::dynamic_pointer_cast<TextGadget>(gadget); gPtr) {
                gPtr->setText(content);
            }
            return *this;
        }

        auto foreground(const Color &color) {
            if (auto gPtr = std::dynamic_pointer_cast<TextGadget>(gadget); gPtr) {
                gPtr->setForeground(color);
            }
            return *this;
        }

        auto pointSize(int size) {
            if (auto gPtr = std::dynamic_pointer_cast<TextGadget>(gadget); gPtr) {
                gPtr->setPointSize(size);
            }
            return *this;
        }
    };

} // rose

#endif //ROSE2_TEXTGADGET_H
