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
#include <functional>
#include <stack>
#include <queue>
#include <ranges>

namespace rose {

/**
 * @class Window
 */
    class Window {
    private:
        SdlWindow mSdlWindow{};
        Context mContext{};
        std::vector<Rectangle> mDisplayBounds{};

        std::vector<std::shared_ptr<Gadget>> mGadgets{};

        std::vector<std::shared_ptr<Gadget>> mFocusChain{};

    public:
        Window() = default;

        Window(const Window&) = delete;
        Window(Window &&) = default;
        Window& operator=(const Window &) = delete;
        Window& operator=(Window &&) = default;

        ~Window() = default;

        Context& context() { return mContext; }

        void draw();

        auto emplace_back(const std::shared_ptr<Gadget>& gadget) { return mGadgets.emplace_back(gadget); }

        /**
         * @brief Clear focus chain.
         * @details Each gadget on the focus chain has their focus flag set to false. The focus chain is cleared.
         */
        void clearFocusChain();

        /**
         * @brief The specified gadget, and all its managers have their focus flag set and are pushed onto
         * the back of the focus chain.
         * @details This results in the focus chain representing a bottom up list. A Window can search the
         * list forward to find a gadget that is able to accept a focus related event.
         * @param gadget the lowest gadget to be added to the chain.
         */
        [[maybe_unused]] void setFocusGadget(std::shared_ptr<Gadget> &gadget);

        /**
         * @brief Preorder traversal of a Widget tree applying a lambda to all Gadgets.
         * @details The tree is traversed starting from the specified Gadget. Gadgets which are also Widgets are
         * traversed in turn. The provided lambda is applied to all Gadgets. Traversal preorder.
         * @param topGadget The Gadget to start traversal from.
         * @param lambda The lambda to apply.
         */
        static void gadgetTraversal(std::shared_ptr<Gadget> &topGadget,
                                    const std::function< void(std::shared_ptr<Gadget>&) >& lambda);

        /**
         * @brief Clear all focus flags for Gadgets attached to this Window using gadgetTraversal.
         */
        [[maybe_unused]] [[maybe_unused]] void clearAllFocusFlags();

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
