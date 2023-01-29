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

    /**
     * @brief Exception thrown when an unrecoverable error is encountered by the TextGadget.
     */
    class TextGadgetException : public std::runtime_error {
    public:
        explicit TextGadgetException(const std::string &what_arg) : std::runtime_error(what_arg) {}

        [[maybe_unused]] explicit TextGadgetException(const char *what_arg) : std::runtime_error(what_arg) {}
    };

    /**
     * @brief Exception thrown when the Font cache encounters an unrecoverable error.
     */
    class FontCacheException : public std::runtime_error {
    public:
        [[maybe_unused]] explicit FontCacheException(const std::string &what_arg) : std::runtime_error(what_arg) {}

        explicit FontCacheException(const char *what_arg) : std::runtime_error(what_arg) {}
    };

    /**
     * @class TextGadget
     * @brief A Gadget that displays a text string.
     */
    class [[maybe_unused]] TextGadget : public Gadget {
        friend class TextGadgetBuilder;

    protected:
        constexpr static std::string_view ClassName = "TextGadget";     ///< TextGadget class name.

        /**
         * @brief Global pointer to the font cache for the text fonts used in the application.
         */
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
        RenderStyle mRenderStyle{RenderStyle::Blended};          ///< The style of rendering Solid, Shaded, or Blended.
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

        /**
         * @brief A static method to initialize the global text font cache.
         * @tparam String The type of fontSearchPaths.
         * @param fontSearchPaths A colon ':' separated list of search paths.
         */
        template<class String>
        requires StringLike<String>
        static void InitializeFontCache(String fontSearchPaths) {
            mFontCache = std::make_unique<FontCache>(fontSearchPaths);
        }

        /**
         * @brief A static method to get a font through the global text font cache.
         * @details If the provided font name is not found the method will fall back to "FreeSans".
         * @tparam String The type of fontName.
         * @param fontName The name of the font to search for and return.
         * @param pointSize The point size of the font to return.
         * @return A FontPointer to the returned font.
         */
        template<class String>
        requires StringLike<String>
        static FontPointer getFont(String fontName, int pointSize) {
            if (mFontCache){
                if (auto fontPointer = mFontCache->getFont(fontName, pointSize); fontPointer) {
                    return fontPointer;
                } else if (auto defaultFontPointer = mFontCache->getFont("FreeSans", pointSize); defaultFontPointer) {
                    return defaultFontPointer;
                }
                throw FontCacheException(fmt::format("Could not find requested font '{}' nor 'FreeSans'", fontName));
            }

            throw FontCacheException("TextGadget font cache not initialized.");
        }

        /**
         * @brief Get the class name.
         * @return std::string_view &
         */
        const std::string_view& className() const override { return TextGadget::ClassName; }

        bool initialLayout(Context &context) override;

        void draw(Context &context, Point drawLocation) override;

        void expose(Context &context, Rectangle exposed) override;

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

        /**
         * @brief Called when the text is updated.
         * @details Sets: needs drawing, needs layout, text render required and resets the Texture.
         */
        void textUpdated();

        /**
         * @brief Sets the text point size.
         * @details If the size is a change textUpdated() is called.
         * @param pointSize the new point size.
         */
        void setPointSize(int pointSize) {
            if (mPointSize != pointSize) {
                mPointSize = pointSize;
                textUpdated();
            }
        }

        /**
         * @brief Sets the text foreground color.
         * @details If the color changes textUpdated() is called.
         * @param color the new color.
         */
        void setForeground(Color color) {
            if (mTextFgColor != color) {
                mTextFgColor = color;
                textUpdated();
            }
        }

        /**
         * @brief Set the font named.
         * @details If the font name changes textUpdated() is called.
         * @tparam S the fontName type.
         * @param fontName the new font name.
         */
        template<class S>
        requires StringLike<S>
        void setFontName(const S &fontName) {
            if (mFontName != fontName) {
                mFontName = fontName;
                textUpdated();
            }
        }

        /**
         * @brief Set the text string.
         * @details If the text changes textUpdated() is called.
         * @tparam S the type of text.
         * @param text the new text string.
         */
        template<class S>
        requires StringLike<S>
        void setText(const S &text) {
            if (mText != text) {
                mText = text;
                textUpdated();
            }
        }

        const FontPointer& getFont() const { return mFont; }
    };

    /**
     * @brief Set the text value on a rose::TextGadget
     * @tparam Parm The param::Text type.
     * @param gadget Pointer to rose::TextGadget.
     * @param parameter The param::Text value.
     */
    template<class Parm>
    requires std::is_same_v<Parm,Parameter<MetaType::Text,std::string>>
    [[maybe_unused]] void setEnumParameter(std::shared_ptr<TextGadget>& gadget, Parm parameter) {
        gadget->setText(parameter.data);
    }

    /**
     * @brief Report errors in Icon code point usage.
     */
    class CodePointError : public std::runtime_error {
    public:
        explicit CodePointError(const std::string& whatArg) : std::runtime_error(whatArg) {}
        explicit CodePointError(const char *whatArg) : std::runtime_error(whatArg) {}
    };

    /**
     * @class IconGadget
     * @brief Display a Material icon font element.
     */
    class IconGadget : public TextGadget {
    protected:
        /**
         * @brief Global pointer to Material icon font.
         * https://fonts.google.com/icons
         */
        static std::unique_ptr<Material> mMaterial;

        constexpr static std::string_view ClassName = "IconGadget";     ///< Class name.
        uint32_t mIconCode{};                                           ///< The codepoint.

    public:
        /**
         * @brief Static method to initialize the global storage of the Material Icon font.
         * @tparam String1 the type of fontSearchPaths
         * @tparam String2 the type of fontName
         * @param fontSearchPaths A colon ':' separated list of search paths.
         * @param fontName The name of the Material Icon font to use.
         */
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

        void initialize() override;

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

        /**
         * @brief Set the icon codepoint.
         * @details If the codepoint changes textUpdated() is called.
         * @param icon the codepoint.
         */
        void setIcon(uint32_t icon) {
            if (mIconCode != icon) {
                mIconCode = icon;
                textUpdated();
            }
        }

        /**
         * @brief Convert a code point name into a code point.
         * @tparam String The type of the code point name.
         * @param codePointName The code point name.
         * @return the code point.
         */
        template<class String>
                requires StringLike<String>
        static uint32_t getIcon(const String& codePointName) {
            if (auto itr = mMaterial->find(std::string{codePointName}); itr != mMaterial->end()) {
                return itr->second;
            }
            throw CodePointError(fmt::format("Code point error: code point '{}' not found.", codePointName));
        }

        /**
         * @brief Set the icon codepoint from a code point name.
         * @details The code point names are read in from the codepoints file that comes along with the font file.
         * the code point is set by calling setIcon(uint32_t icon).
         * @tparam String the type of codePointName.
         * @param codePointName The name of the code point to use.
         */
        template<class String>
                requires StringLike<String>
        void setIcon(String codePointName) {
            if (auto itr = mMaterial->find(codePointName); itr != mMaterial->end()) {
                setIcon(itr->second);
            }
        }

        uint32_t getIconCode() const { return mIconCode; }
    };

} // rose

#endif //ROSE2_TEXTGADGET_H
