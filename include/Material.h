//
// Created by richard on 20/01/23.
//

/*
 * Material.h Created by Richard Buckley (C) 20/01/23
 */

/**
 * @file Material.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 20/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_MATERIAL_H
#define ROSE2_MATERIAL_H

#include <Rose.h>
#include <map>
#include <string_view>
#include <Font.h>
#include <filesystem>
#include <vector>
#include <sstream>

namespace rose {

    /**
     * @class FontManager
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

        template<class String>
                requires StringLike<String>
        explicit FontManager(String string) {
            std::stringstream strm(string);
            std::string rootPathStr{};
            while (getline(strm, rootPathStr, ':')) {
                emplace_back(rootPathStr);
            }
        }

        template<typename String>
                requires StringLike<String>
        std::optional<std::filesystem::path> locateFont(const std::filesystem::path &path, String fontName) {
            for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
                if (p.path().stem() == fontName && p.is_regular_file()) {
                    auto ext = p.path().extension().string();
                    if (ext == ".ttf" || ext == ".otf")
                        return p.path();
                }
            }
            return std::nullopt;
        }

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
     * @class Material
     */
    class Material : public std::map<std::string, unsigned int> {
    protected:
        FontManager mFontManager{};
        std::string mFontName{};

        void loadCodePoints(const std::filesystem::path& codePointsFile);

    public:
        template<class String1, class String2>
                requires StringLike<String1> && StringLike<String2>
        explicit Material(String1 fontSearchPaths, String2 fontName) : mFontManager(fontSearchPaths) {
            mFontName = fontName;
            std::optional<std::filesystem::path> fontPath = mFontManager.getFontPath(fontName);
            if (fontPath) {
                fontPath.value().replace_extension("codepoints");
                fmt::print("{}\n", fontPath.value().string());
                if (exists(fontPath.value()) && is_regular_file(fontPath.value())) {
                    loadCodePoints(fontPath.value());
                }
            }
        }

        FontPointer getFont(int pointSize) {
            auto fontPointer = mFontManager.getFont(mFontName, pointSize);
            return fontPointer;
        }
    };

} // rose

#endif //ROSE2_MATERIAL_H
