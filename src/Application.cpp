//
// Created by richard on 01/01/23.
//

/*
 * Application.cpp Created by Richard Buckley (C) 01/01/23
 */

#include "Application.h"
#include <regex>
#include <filesystem>

namespace rose {
    Application::Application(int argc, char **argv) : inputParser(argc, argv) {

        // Determine if a keyboard is attached.
        std::regex kbPathRegEx{std::string{KeyboardPathRegEx}.c_str()};
        for(auto& p: std::filesystem::directory_iterator(UsbDeviceByPath)) {
            if (mKeyboardFound = std::regex_match(p.path().string(), kbPathRegEx); mKeyboardFound)
                break;
        }

    }
} // rose