//
// Created by richard on 02/01/23.
//

/*
 * Window.cpp Created by Richard Buckley (C) 02/01/23
 */

#include "Window.h"

namespace rose {

    void Window::layout() {
        /**
         * Layout stage one.
         */
        Rectangle unset{};
        for (const auto& screen : mScreens)
            screen->layout(context(), unset);

        /**
         * Layout stage two.
         */
        Size sdlWindowSize{};
        Point point{0,0};
        SDL_GetWindowSize(mSdlWindow.get(), &sdlWindowSize.w, &sdlWindowSize.h);
        sdlWindowSize.set = true;
        for (const auto& screen : mScreens)
            screen->layout(context(), Rectangle{point, sdlWindowSize});
    }

    void Window::draw() {
        if (!mScreens.empty())
            mScreens.front()->draw(context());
    }

    [[maybe_unused]] void Window::setFocusGadget(std::shared_ptr<Gadget> &gadget) {
        clearFocusChain();
        while (gadget) {
            gadget->mHasFocus = true;
            mFocusChain.push_back(gadget);
            gadget = gadget->manager.lock();
        }
    }

    void Window::clearFocusChain() {
        for (auto &gadget : mFocusChain) {
            gadget->mHasFocus = false;
        }

        mFocusChain.clear();
    }

    [[maybe_unused]] void Window::clearAllFocusFlags() {
        for (auto &screen : mScreens) {
            gadgetTraversal(screen, [](std::shared_ptr<Gadget> &g) { g->mHasFocus = false; });
        }
    }

    void Window::gadgetTraversal(std::shared_ptr<Screen> &topGadget,
                                 const std::function<void(std::shared_ptr<Gadget> &)> &lambda) {

        std::stack<std::shared_ptr<Gadget>> stack{};
        stack.push(topGadget);

        while (!stack.empty()) {
            auto gadget = stack.top();
            stack.pop();
            lambda(gadget);
            if (auto widget = std::dynamic_pointer_cast<Widget>(gadget); widget) {
                std::ranges::reverse_view reverseGadgetList{widget->mGadgetList};
                for (auto &newGadget : reverseGadgetList) {
                    stack.push(newGadget);
                }
            }
        }
    }

    void
    Window::initialize(const std::shared_ptr<Application>& applicationPtr, const std::string &title, Size initialSize,
                       const Point &initialPosition, uint32_t extraFlags) {
        mApplicationPtr = applicationPtr;
        uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        // Create an application window with the following settings:
        mSdlWindow.reset(SDL_CreateWindow(
                (title.empty() ? "SDL2 window" : std::string{title}.c_str()),         //    const char* title
                initialPosition.x,        //    int x: initial x position
                initialPosition.y,        //    int y: initial y position
                initialSize.w,            //    int w: width, in pixels
                initialSize.h,            //    int h: height, in pixels
                flags | extraFlags
        ));

        if (mSdlWindow) {
            SDL_version sdlVersion;
            SDL_GetVersion(&sdlVersion);
            fmt::print("   Number of displays: {}, On: {}\n", SDL_GetNumVideoDisplays(),
                       SDL_GetWindowDisplayIndex(mSdlWindow.get()));
            fmt::print("   Version: {}.{}.{}\n", (int) sdlVersion.major, (int) sdlVersion.minor, (int) sdlVersion.patch);
            for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i) {
                SDL_Rect displayBounds{0, 0, 0, 0};

                if (SDL_GetDisplayBounds(i, &displayBounds)) {
                    mDisplayBounds.emplace_back();
                } else {
                    mDisplayBounds.emplace_back(
                            displayBounds.x, displayBounds.y, displayBounds.w, displayBounds.h);
                    fmt::print("   Display {}: {:o}\n", i, mDisplayBounds.back());
                }
            }

            mContext = Context{mSdlWindow, -1, RendererFlags::RENDERER_ACCELERATED
                                               | RendererFlags::RENDERER_TARGETTEXTURE
                                               | RendererFlags::RENDERER_PRESENTVSYNC};

            if (mContext) {
                mContext.setDrawBlendMode(SDL_BLENDMODE_BLEND);
            } else {
                throw ContextException(fmt::format("Could not create SDL_Renderer: {}", SDL_GetError() ));
            }

            auto screen = std::make_shared<Screen>(shared_from_this(), initialSize);
            mScreens.emplace_back(std::move(screen));
            mScreens.back()->setLayoutManager(std::make_unique<LayoutManager>());
        } else {
            std::string sdlError{SDL_GetError()};
            if (sdlError == "Could not initialize EGL") {
                throw std::runtime_error("If X11 is running, check DISPLAY environment variable.");
            }
            throw std::runtime_error(fmt::format("Could not create SDL_Window: {}", sdlError));
        }
    }
} // rose