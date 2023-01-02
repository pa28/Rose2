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

            applicationDraw(mGraphicsModel.context());

            fps.next();
        }
    }

    void Application::applicationDraw(Context& context) {
        context.setDrawColor(0x00, 0x7f, 0x00, 0xff);
        context.renderClear();
        context.setDrawColor(0x00, 0x00, 0x7f, 0xff);
        context.fillRect(Rectangle(120, 160, 680, 440));
        context.renderPresent();
    }

    bool Application::initializeGraphics(uint32_t extraFlags) {
        if (!mWidowSizePos)
            mWidowSizePos = Rectangle(100,100,800,600);
        if (mWindowName.empty()) {
            std::filesystem::path appPath{mInputParser.programPathName};
            mWindowName = appPath.filename().string();
        }
        return mGraphicsModel.initialize(mWindowName, mWidowSizePos.size, mWidowSizePos.point, extraFlags);
    }

} // rose