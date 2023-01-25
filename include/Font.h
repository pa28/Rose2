//
// Created by richard on 03/01/23.
//

/*
 * Font.h Created by Richard Buckley (C) 03/01/23
 */

/**
 * @file Font.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 2.0
 * @date 21/01/23
 * @brief Font management.
 * @details Software used in finding, loading, and drawing text with fonts.
 */

#ifndef ROSE2_FONT_H
#define ROSE2_FONT_H

#include <memory>

#include <SDL2/SDL_ttf.h>
#include <map>
#include <vector>
#include <filesystem>
#include <sstream>
#include <optional>
#include <fmt/format.h>
#include <Rose.h>

namespace rose {

    /**
     * @class FontDestroy
     * @brief A functor to destroy a TTF_Font
     */
    class FontDestroy {
    public:
        /**
         * @brief Destroy a TTF_Font pointer
         * @param ttfFont the Font pointer
         */
        void operator()(TTF_Font *ttfFont) {
            TTF_CloseFont(ttfFont);
        }
    };

    using FontPointer = std::shared_ptr<TTF_Font>;                  ///< Type for TTF smart pointer
    using FontCacheKey = std::pair<std::string, int>;               ///< Type for TTF cache key
    using FontCacheStore [[maybe_unused]] = std::map<FontCacheKey, FontPointer>;     ///< Type for TTF cache store

    /**
     * @brief Get the size of a UTF8 string.
     * @param fontPointer The font to use.
     * @param text The string
     * @return A std::tuple<int,int> with the width and height of the text.
     */
    [[maybe_unused]] inline auto textSizeUTF8(FontPointer &fontPointer, const std::string &text) {
        int w, h;
        TTF_SizeUTF8(fontPointer.get(), text.c_str(), &w, &h);
        return std::make_tuple(w, h);
    }

    /**
     * @class FontManager
     * @brief Manage requests for Fonts by locating them in a specified set of filesystem paths.
     */
    class FontManager : public std::vector<std::filesystem::path> {
    protected:
        std::map<std::string, std::filesystem::path> mFontPathMap{};  ///< The font file path cache

        std::map<FontCacheKey, FontPointer> mFontCache{};             ///< The font cache

    public:
        FontManager() = default;
        FontManager(const FontManager&) = delete;
        FontManager(FontManager&&) = default;
        FontManager& operator=(const FontManager&) = delete;
        FontManager& operator=(FontManager&&) = default;
        ~FontManager() = default;

        /**
         * @brief Create a FontManager with a set of colon ':' delimited font paths.
         * @tparam String The type of the font path specification.
         * @param string A set of font paths.
         */
        template<class String>
        requires StringLike<String>
        explicit FontManager(String string) {
            std::stringstream strm(string);
            std::string rootPathStr{};
            while (getline(strm, rootPathStr, ':')) {
                emplace_back(rootPathStr);
            }
        }

        /**
         * @brief Locate a font by recursively iterating from a file system path.
         * @details This method will only find True Type and Open Type fonts with extensions .ttf, .otf, .afm
         * @tparam String The type of the font name.
         * @param path The file system path to start the search from.
         * @param fontName The name of the font to find.
         * @return The filesystem path to the located font.
         */
        template<typename String>
        requires StringLike<String>
        std::optional<std::filesystem::path> locateFont(const std::filesystem::path &path, String fontName) {
            for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
                if (p.path().stem() == fontName && p.is_regular_file()) {
                    auto ext = p.path().extension().string();
                    if (ext == ".ttf" || ext == ".otf" || ext == ".afm" || ext == ".t1" || ext == ".pfb")
                        return p.path();
                }
            }
            return std::nullopt;
        }

        /**
         * @brief Get a path to a font from the cache or from the filesystem.
         * @details The font cache is search first, if not found the filesystem is searched using locateFont().
         * If the font is found on the filesystem it is added to the cache.
         * @tparam String The type of the font name.
         * @param fontName The font name
         * @return std::optional<std::filesystem::path>
         */
        template<class String>
        requires StringLike<String>
        std::optional<std::filesystem::path> getFontPath(String fontName) {
            if (auto found = mFontPathMap.find(fontName); found != mFontPathMap.end())
                return found->second;

            for (auto const &rootPath : *this) {
                auto fontPath = locateFont(rootPath, fontName);
                if (fontPath) {
                    mFontPathMap[fontName] = fontPath.value();
                    return fontPath;
                }
            }

            return std::nullopt;
        }

        /**
         * @brief Get a pointer to a font specified by name and point size.
         * @details The filesystem is searched using getFontPath().
         * @tparam StringType The type of the font name.
         * @param fontName The font name.
         * @param ptSize The point size.
         * @return a FontPointer which may be nullptr if the font is not found.
         */
        template<typename StringType>
        FontPointer getFont(StringType fontName, int ptSize) {
            if (auto found = mFontCache.find(FontCacheKey{fontName, ptSize}); found != mFontCache.end()) {
                return found->second;
            }

            if (auto fontPath = getFontPath(fontName); fontPath) {
                FontPointer fontPointer{TTF_OpenFont(fontPath.value().c_str(), ptSize), FontDestroy{}};
                if (fontPointer) {
                    auto font = mFontCache.emplace(FontCacheKey{fontName, ptSize}, fontPointer);
                    if (font.second) {
                        return font.first->second;
                    }
                } else {
                    fmt::print("TTF_OpenFont error: {}\n", SDL_GetError());
                }
            }

            return nullptr;
        }
    };

    /**
     * @class FontCache
     * @brief Cache storage for requested fonts.
     */
    class FontCache {
    protected:
        std::vector<std::filesystem::path> mFontPathList{};

        FontManager mFontManager;

        FontCache() {
            std::stringstream strm("/home/richard/CLionProjects/material-design-icons/font:/usr/share/fonts:/usr/local/share/fonts");
            std::string rootPathStr{};
            while (getline(strm, rootPathStr, ':')) {
                mFontPathList.emplace_back(rootPathStr);
            }
        }

    public:
        template<class String>
        requires StringLike<String>
        explicit FontCache(String fontSearchPaths) : mFontManager(fontSearchPaths) {}

        template<class String>
        requires StringLike<String>
        FontPointer getFont(String fontName, int pointSize) {
            auto fontPointer = mFontManager.getFont(fontName, pointSize);
            return fontPointer;
        }
    };

    /**
     * @struct FontMetrics
     * @brief The size metrics that pertain to a particular font.
     */
    struct FontMetrics {
        int fontAscent{},     ///< The height above the base line.
        fontDescent{},        ///< The length of descenders below the baseline a negative number.
        fontHeight{},         ///< The total height of the font (ascent - descent
        fontLineSkip{};       ///< The size of a line advance for the font.

        FontMetrics() = default;
        constexpr FontMetrics(const FontMetrics &) noexcept = default;
        constexpr FontMetrics(FontMetrics &&) noexcept = default;
        FontMetrics& operator=(const FontMetrics &) noexcept = default;
        FontMetrics& operator=(FontMetrics &&) noexcept = default;
    };

    /**
     * @brief Get the font metrics of the current font.
     * @details See <a href="https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_29.html#SEC29">TTF_FontHeight</a>
     * et al.
     * @return a std::tuple with font height, font ascent, font descent, and font line skip.
     */
    [[maybe_unused]] inline auto getFontMetrics(const FontPointer& font) {
        FontMetrics fontMetrics{};
        fontMetrics.fontHeight = TTF_FontHeight(font.get());
        fontMetrics.fontAscent = TTF_FontAscent(font.get());
        fontMetrics.fontDescent = TTF_FontDescent(font.get());
        fontMetrics.fontLineSkip = TTF_FontLineSkip(font.get());
        return fontMetrics;
    }

    struct GlyphMetrics {
        FontMetrics fontMetrics{};
        int minX{}, maxX{}, minY{}, maxY{}, advance{};

        GlyphMetrics() = default;
        GlyphMetrics(const GlyphMetrics&) = default;
        GlyphMetrics(GlyphMetrics&&) = default;
        GlyphMetrics& operator=(const GlyphMetrics&) = default;
        GlyphMetrics& operator=(GlyphMetrics&&) = default;
    };

    [[maybe_unused]] inline GlyphMetrics getGlyphMetrics(const FontPointer &font, Uint16 glyph) {
        GlyphMetrics gm{};
        if (font) {
            gm.fontMetrics.fontHeight = TTF_FontHeight(font.get());
            gm.fontMetrics.fontAscent = TTF_FontAscent(font.get());
            gm.fontMetrics.fontDescent = TTF_FontDescent(font.get());
            gm.fontMetrics.fontLineSkip = TTF_FontLineSkip(font.get());
            TTF_GlyphMetrics(font.get(), glyph, &gm.minX, &gm.maxX, &gm.minY, &gm.maxY, &gm.advance);
        }
        return gm;
    }

    [[maybe_unused]] inline GlyphMetrics getGlyphMetrics32(const FontPointer &font, Uint32 glyph) {
        GlyphMetrics gm{};
        if (font) {
            gm.fontMetrics.fontHeight = TTF_FontHeight(font.get());
            gm.fontMetrics.fontAscent = TTF_FontAscent(font.get());
            gm.fontMetrics.fontDescent = TTF_FontDescent(font.get());
            gm.fontMetrics.fontLineSkip = TTF_FontLineSkip(font.get());
            TTF_GlyphMetrics32(font.get(), glyph, &gm.minX, &gm.maxX, &gm.minY, &gm.maxY, &gm.advance);
        }
        return gm;
    }


#if 0
    /**
     * @brief Fetch a font.
     * @details If the font at the size requested is in the cache, the cached value is returned. If the font
     * can't be found the default font "FreeSans" is tried. If that can't be found and exception is thrown.
     * @param fontCache The FontCache.
     * @param fontName The name of the font.
     * @param fontSize The point size of the font in pixels.
     * @return The font.
     */
    [[maybe_unused]] inline auto fetchFont(FontCache &fontCache, const std::string &fontName, int fontSize) {
        auto font = fontCache.getFont(fontName, fontSize);
        if (!font) {
            font = fontCache.getFont("FreeSans", fontSize);
            if (!font)
                throw std::runtime_error(fmt::format("Neither font {} nor default font 'FreeSans1 found.", fontName));
        }
        return font;
    }
#endif

} // rose

namespace fmt {
    template<>
    class formatter<rose::FontMetrics> {
        char presentation_ = 'd';
    public:
        // parse format and store it
        constexpr auto parse(format_parse_context &ctx) {
            auto i = ctx.begin(), end = ctx.end();
            if (i != end && (*i == 'd' || *i == 'o')) {
                presentation_ = *i++;
            }
            if (i != end && *i != '}') {
                throw format_error("Invalid rectangle format.");
            }
            return i;
        }

        // format a value using stored specification:
        template<class FmtContext>
        constexpr auto format(const rose::FontMetrics &fm, FmtContext &ctx) const {
            // note: we can't use ternary operator '?:' in a constexpr
            switch (presentation_) {
                default:
                    // 'ctx.out()' is an output iterator
                case 'd':
                    return format_to(ctx.out(), "[A{:d}, D{:d}, H{:d}, Ls{:d}]",
                                     fm.fontAscent, fm.fontDescent, fm.fontHeight, fm.fontLineSkip);
            }
        }
    };

    template<>
    class formatter<rose::GlyphMetrics> {
        char presentation_ = 'd';
    public:
        // parse format and store it
        constexpr auto parse(format_parse_context &ctx) {
            auto i = ctx.begin(), end = ctx.end();
            if (i != end && (*i == 'd' || *i == 'o')) {
                presentation_ = *i++;
            }
            if (i != end && *i != '}') {
                throw format_error("Invalid rectangle format.");
            }
            return i;
        }

        // format a value using stored specification:
        template<class FmtContext>
        constexpr auto format(const rose::GlyphMetrics &gm, FmtContext &ctx) const {
            // note: we can't use ternary operator '?:' in a constexpr
            switch (presentation_) {
                default:
                    // 'ctx.out()' is an output iterator
                case 'd':
                    return format_to(ctx.out(), "[Font: {}; x{:d}, X{:d}, y{:d}, Y{:d}, adv{:d}]",
                                     gm.fontMetrics, gm.minX, gm.maxX, gm.minY, gm.maxY, gm.advance);
            }
        }
    };
} // fmt

#endif //ROSE2_FONT_H
