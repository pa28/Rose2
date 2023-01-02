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
    Application::Application(int argc, char **argv) : mInputParser(argc, argv) {
        // Determine if a keyboard is attached.
        std::regex kbPathRegEx{std::string{KeyboardPathRegEx}.c_str()};
        for(auto& p: std::filesystem::directory_iterator(UsbDeviceByPath)) {
            if (mKeyboardFound = std::regex_match(p.path().string(), kbPathRegEx); mKeyboardFound)
                break;
        }
    }

    void Application::basicEventLoop() {
        SDL_Event e;
        Fps fps;

        while (mRunEventLoop) {
            //Handle events on queue
            while (SDL_PollEvent(&e) != 0) {
                //User requests quit
                if (e.type == SDL_QUIT) {
                    mRunEventLoop = false;
                    continue;
                }
            }

            applicationDraw();

            fps.next();
        }
    }

    void Application::applicationDraw() {
        for (const auto &window : mWindows) {
            window->draw();
//            window->context().setDrawColor(0x00, 0x7f, 0x00, 0xff);
//            window->context().renderClear();
//            window->context().setDrawColor(0x00, 0x00, 0x7f, 0xff);
//            window->context().fillRect(Rectangle(120, 160, 680, 440));
            window->context().renderPresent();
        }
    }

    bool Application::initializeGraphics() {
        if (!mWidowSizePos) {
            mWidowSizePos = Rectangle(static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(1)),
                                      static_cast<int>(SDL_WINDOWPOS_CENTERED_DISPLAY(1)),
                                      800, 600);
        }
        if (mWindowName.empty()) {
            std::filesystem::path appPath{mInputParser.programPathName};
            mWindowName = appPath.filename().string();
        }
        return mGraphicsModel.initialize();
    }

    std::string Application::applicationName() const {
        std::filesystem::path appPath = mInputParser.programPathName;
        return appPath.filename().string();
    }

} // rose