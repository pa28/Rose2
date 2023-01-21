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
#include <entypo.h>
#include <exception>
#include <Material.h>

namespace rose {

    class TextGadgetException : public std::runtime_error {
    public:
        explicit TextGadgetException(const std::string &what_arg) : std::runtime_error(what_arg) {}

        [[maybe_unused]] explicit TextGadgetException(const char *what_arg) : std::runtime_error(what_arg) {}
    };

    class FontCacheException : public std::runtime_error {
    public:
        [[maybe_unused]] explicit FontCacheException(const std::string &what_arg) : std::runtime_error(what_arg) {}

        explicit FontCacheException(const char *what_arg) : std::runtime_error(what_arg) {}
    };

    /**
     * @class TextGadget
     */
    class [[maybe_unused]] TextGadget : public Gadget {
        friend class TextGadgetBuilder;

    protected:
        constexpr static std::string_view ClassName = "TextGadget";

        static std::unique_ptr<FontCache> mFontCache;

        bool mTextRenderRequired{true};      ///< True when re-rendering of text is required
        Texture mTexture{};                  ///< The generated Texture.
        Size mTextSize{};                    ///< The size of the Texture in pixels.
        std::shared_ptr<_TTF_Font> mFont{};  ///< The cached font used.
        std::string mText{};                 ///< The string to render.

        /**
         * Theme set values.
         */
        Color mTextFgColor{};                ///< The foreground color to use.
        RenderStyle mRenderStyle{};          ///< The style of rendering Solid, Shaded, or Blended.
        std::string mFontName{};             ///< The name of the True Type Font to use.
        ScreenCoordType mPointSize{};        ///< The point (pixel) size of the font.

    public:
        TextGadget() = default;
        explicit TextGadget(std::shared_ptr<Theme>& theme);
        TextGadget(const TextGadget&) = delete;
        TextGadget(TextGadget&&) = default;
        TextGadget& operator = (const TextGadget&) = delete;
        TextGadget& operator = (TextGadget&&) = default;
        ~TextGadget() override = default;

        template<class String>
        requires StringLike<String>
        static void InitializeFontCache(String fontSearchPaths) {
            mFontCache = std::make_unique<FontCache>(fontSearchPaths);
        }

        template<class String>
        requires StringLike<String>
        static FontPointer getFont(String fontName, int pointSize) {
            if (mFontCache){
                if (auto fontPointer = mFontCache->getFont(fontName, pointSize); fontPointer) {
                    return fontPointer;
                } else if (fontPointer = mFontCache->getFont("FreeSans", pointSize); fontPointer) {
                    return fontPointer;
                }
                throw FontCacheException(fmt::format("Could not find requested font '{}' nor 'FreeSans'", fontName));
            }

            throw FontCacheException("TextGadget font cache not initialized.");
        }

        const std::string_view& className() const override { return TextGadget::ClassName; }

        bool initialLayout(Context &context) override;

        void draw(Context &context, Point drawLocation) override;

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
        explicit TextGadgetBuilder(std::shared_ptr<Theme>& theme) : GadgetBuilder(std::make_shared<TextGadget>(theme)) {}
        explicit TextGadgetBuilder(std::shared_ptr<Gadget> g) : GadgetBuilder(std::move(g)) {}

        template<class S>
        requires StringLike<S>
        auto text(S content) {
            if (auto gPtr = std::dynamic_pointer_cast<TextGadget>(gadget); gPtr) {
                gPtr->setText(content);
            }
            return *this;
        }

        [[maybe_unused]] auto foreground(const Color &color) {
            if (auto gPtr = std::dynamic_pointer_cast<TextGadget>(gadget); gPtr) {
                gPtr->setForeground(color);
            }
            return *this;
        }

        [[maybe_unused]] auto pointSize(int size) {
            if (auto gPtr = std::dynamic_pointer_cast<TextGadget>(gadget); gPtr) {
                gPtr->setPointSize(size);
            }
            return *this;
        }

        template<class S>
        requires StringLike<S>
        [[maybe_unused]] auto setFontName(S fontName) {
            if (auto gPtr = std::dynamic_pointer_cast<TextGadget>(gadget); gPtr) {
                gPtr->setFontName(fontName);
            }
            return *this;
        }
    };

    class IconGadget : public TextGadget {
    protected:
        static std::unique_ptr<Material> mMaterial;

        constexpr static std::string_view ClassName = "IconGadget";
        uint32_t mIconCode{ENTYPO_ICON_ROCKET};

    public:
        template<class String1, class String2>
                requires StringLike<String1> && StringLike<String2>
        static void InitializeMaterial(String1 fontSearchPaths, String2 fontName) {
            mMaterial = std::make_unique<Material>(fontSearchPaths, fontName);
        }

        IconGadget() = default;
        explicit IconGadget(std::shared_ptr<Theme>& theme);
        IconGadget(const IconGadget&) = delete;
        IconGadget(IconGadget&&) = default;
        IconGadget& operator = (const IconGadget&) = delete;
        IconGadget& operator = (IconGadget&&) = default;
        ~IconGadget() override = default;

        const std::string_view& className() const override { return IconGadget::ClassName; }

        bool initialLayout(Context &context) override;

        void draw(Context &context, Point drawLocation) override;

        /**
         * @brief Create a Blended Texture from text.
         * @details Fetches the Font corresponding to mFontName and mPointSize, then renders the text in mText as
         * UTF8 to mTexture. The foreground color is set to mTextFgColor. If mRenderStyle is set to Shaded the
         * background color is set to mTextBgColor. The size of the Texture is placed in mTextSize.<p/>
         * If the requested font is not found, or mText is empty any mTexture is reset and mTextSize is set to Zero.
         * @param context The graphics Context.
         * @throws TextGadgetException
         */
        void createIconTexture(Context &context);

        void setIcon(uint32_t icon) {
            mIconCode = icon;
            setNeedsDrawing();
        }

        template<class String>
                requires StringLike<String>
        void setIcon(String codePointName) {
            if (auto itr = mMaterial->find(codePointName); itr != mMaterial->end()) {
                mIconCode = (*itr).second;
                setNeedsDrawing();
            }
        }
    };

    /**
     * @class IconGadgetBuilder
     */
    class [[maybe_unused]] IconGadgetBuilder : public TextGadgetBuilder {
    public:

        IconGadgetBuilder() : TextGadgetBuilder(std::make_shared<IconGadget>()) {}
        explicit IconGadgetBuilder(std::shared_ptr<Theme>& theme) : TextGadgetBuilder(std::make_shared<IconGadget>(theme)) {}

        [[maybe_unused]] explicit IconGadgetBuilder(std::shared_ptr<Gadget> g) : TextGadgetBuilder(std::move(g)) {}

        [[maybe_unused]] auto icon(uint32_t iconCode) {
            if (auto iconGadget = std::dynamic_pointer_cast<IconGadget>(gadget); iconGadget) {
                iconGadget->setIcon(iconCode);
            }
            return *this;
        }

        template<class String>
        requires StringLike<String>
        auto icon(String codePointName) {
            if (auto iconGadget = std::dynamic_pointer_cast<IconGadget>(gadget); iconGadget) {
                iconGadget->setIcon(codePointName);
            }
            return *this;
        }
    };

} // rose

#endif //ROSE2_TEXTGADGET_H
