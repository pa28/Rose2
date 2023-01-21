//
// Created by richard on 20/01/23.
//

/*
 * Material.cpp Created by Richard Buckley (C) 20/01/23
 */

/**
 * @file Material.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 20/01/23
 */

#include <fstream>
#include "Material.h"

namespace rose {
    void Material::loadCodePoints(const std::filesystem::path& codePointsFile) {
        std::ifstream strm;
        if (strm.open(codePointsFile); strm) {
            for (std::string line; std::getline(strm, line);) {
                auto pos = line.find(' ');
                auto name = line.substr(0, pos);
                auto value = line.substr(pos + 1);
                auto point = std::stoul(value, nullptr, 16);
                emplace(name, point);
            }
        }
    }
} // rose