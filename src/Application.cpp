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


        event.setMouseMotion([this](const SDL_MouseMotionEvent &e) -> bool { return handleMouseMotionEvent(e); });

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

    std::shared_ptr<Gadget> Application::mouseMotionEvent(const SDL_MouseMotionEvent &e) {
        if (!mMouseWindow.expired())
            return mMouseWindow.lock()->findGadget([&e](std::shared_ptr<Gadget> &gadget) -> bool {
                auto r = gadget->containsPoint(Point{e.x, e.y});
                return r;
            });
        return std::make_shared<Gadget>();
    }

    void Application::winStateChangeEvent(WindowEventType type, const SDL_WindowEvent &e) {
        switch (type) {
            case Enter:
                for (const auto &window : mWindows) {
                    if (window->windowID() == e.windowID) {
                        mMouseWindow = window->weakPtr();
                    }
                }
                break;
            case Leave:
                for (const auto &window : mWindows) {
                    if (window->windowID() == e.windowID) {
                        mMouseWindow.reset();
                        if (!mMouseGadget.expired()) {
                            mMouseGadget.lock()->enterLeaveEvent(false, e.timestamp);
                            mMouseGadget.reset();
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

} // rose