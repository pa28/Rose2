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

                event.onEvent(e);
            }

            applicationDraw();

            fps.next();
        }
    }

    void Application::applicationDraw() {
        for (const auto &window : mWindows) {
            window->draw();
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


        event.setMouseMotion([this](const SDL_MouseMotionEvent &e) -> bool { return mouseMotionEvent(e); });

        event.setWinStateChange([this](WindowEventType type, const SDL_WindowEvent &e) -> void { winStateChangeEvent(type,e); } );

        return mGraphicsModel.initialize();
    }

    std::string Application::applicationName() const {
        std::filesystem::path appPath = mInputParser.programPathName;
        return appPath.filename().string();
    }

    std::shared_ptr<Application> Application::createApplication(int argc, char **argv) {
        return std::make_shared<Application>(argc, argv);
    }

    bool Application::mouseMotionEvent(const SDL_MouseMotionEvent &e) {
        fmt::print("Mouse motion {} -> {}\n", Point(e.x, e.y), Point(e.xrel, e.yrel));
        return true;
    }

    void Application::winStateChangeEvent(WindowEventType type, const SDL_WindowEvent &e) {
        switch (type) {
            case Enter:
                for (const auto &window : mWindows) {
                    if (window->windowID() == e.windowID) {
                        fmt::print("Window found\n");
                        mMouseWindow.reset();
                        mMouseWindow = window->weakPtr();
                    }
                }
                break;
            case Leave:
                for (const auto &window : mWindows) {
                    if (window->windowID() == e.windowID) {
                        fmt::print("Window found\n");
                        mMouseWindow.reset();
                    }
                }
                break;
            default:
                break;
        }
    }

} // rose