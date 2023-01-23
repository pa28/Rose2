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
        mTheme = std::make_shared<Theme>();
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

            animationSignal.transmit(SDL_GetTicks64());
            if (mNeedsDrawing)
                applicationDraw();

            fps.next();
        }
    }

    void Application::applicationDraw() {
        for (const auto &window : mWindows) {
            window->draw();
            window->context().renderPresent();
        }
        mNeedsDrawing = false;
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

        event.setMouseButton([this](const SDL_MouseButtonEvent &e) -> bool { return handleMouseButtonEvent(e); });

        event.setWinSizeChange([this](WindowEventType windowEventType, const SDL_WindowEvent &e) -> void {
            winSizeChange(windowEventType, e); });

        return rose::GraphicsModel::initialize();
    }

    std::string Application::applicationName() const {
        std::filesystem::path appPath = mInputParser.programPathName;
        return appPath.filename().string();
    }

    std::shared_ptr<Gadget> Application::mousePointerToGadget(const Point &point) {
        if (!mMouseWindow.expired())
            return mMouseWindow.lock()->findGadget([&point](std::shared_ptr<Gadget> &gadget) -> bool {
                auto r = gadget->containsPoint(point);
                return r;
            });
        return std::make_shared<Gadget>();
    }

    std::shared_ptr<Gadget> Application::validateMouseGadget(const Point &point, Uint32 timestamp) {
        if (mMouseGadget.expired()) {
            if (auto gadget = mousePointerToGadget(point); gadget) {
                mMouseGadget = gadget;
                gadget->enterLeaveEvent(true, timestamp);
                return gadget;
            }
        } else if (auto oldGadget = mMouseGadget.lock(); oldGadget) {
            if (auto currentGadget = mousePointerToGadget(point); currentGadget) {
                if (oldGadget != currentGadget) {
                    oldGadget->enterLeaveEvent(false, timestamp);
                    mMouseGadget = currentGadget;
                    currentGadget->enterLeaveEvent(true, timestamp);
                    return currentGadget;
                }
                return oldGadget;
            }
        }
        return nullptr;
    }

    bool Application::handleMouseMotionEvent(const SDL_MouseMotionEvent &e) {
        if ((e.state & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK | SDL_BUTTON_MMASK)) == 0) {
            // Process enter and leave events.
            if (auto gadget = validateMouseGadget(Point{e.x, e.y}, e.timestamp)) {
                return true;
            }
        } else {
            // Process drag events which may generate enter and leave events when drag is not supported.
            if (auto gadget = validateMouseGadget(Point{e.x, e.y}, e.timestamp)) {
                // ToDo: Implement drag event.
                return true;
            }
        }
        return false;
    }

    bool Application::handleMouseButtonEvent(const SDL_MouseButtonEvent &e) {
        if (auto gadget = validateMouseGadget(Point{e.x, e.y}, e.timestamp); gadget) {
            return gadget->mouseButtonEvent(e);
        }
        return false;
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

    void Application::winSizeChange(WindowEventType, const SDL_WindowEvent &e) {
        auto winId = [e](const std::shared_ptr<Window>& w) { return w->windowID() == e.windowID; };
        if (auto window = std::ranges::find_if(mWindows,winId); window != mWindows.end()) {
            (*window)->changeSize(Size{e.data1, e.data2});
            (*window)->layout();
            (*window)->setNeedsDrawing();
        }
    }

} // rose