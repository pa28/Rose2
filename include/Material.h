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
