//
// Created by richard on 02/01/23.
//

/*
 * Window.h Created by Richard Buckley (C) 02/01/23
 */

#ifndef ROSE2_WINDOW_H
#define ROSE2_WINDOW_H

#include <Rose.h>
#include <SDL.h>
#include <GraphicsModel.h>
#include <Gadget.h>
#include <exception>

namespace rose {

/**
 * @class Window
 */
    class Window {
    private:
        SdlWindow mSdlWindow{};
        Context mContext{};
        std::vector<Rectangle> mDisplayBounds{};

        std::vector<std::shared_ptr<Widget>> mWidgets{};

    public:
        Window() = default;

        Window(const Window&) = delete;
        Window(Window &&) = default;
        Window& operator=(const Window &) = delete;
        Window& operator=(Window &&) = default;

        ~Window() = default;

        Context& context() { return mContext; }

        void draw();

        auto emplace_back(const std::shared_ptr<Widget>& widget) { return mWidgets.emplace_back(widget); }

        void initialize(const std::string& title, Size initialSize, const Point &initialPosition,
                                       uint32_t extraFlags) {
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
                SDL_VERSION(&sdlVersion);
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
            } else {
                std::string sdlError{SDL_GetError()};
                if (sdlError == "Could not initialize EGL") {
                    throw std::runtime_error("If X11 is running, check DISPLAY environment variable.");
                }
                throw std::runtime_error(fmt::format("Could not create SDL_Window: {}", sdlError));
            }
        }
    };

} // rose

#endif //ROSE2_WINDOW_H
