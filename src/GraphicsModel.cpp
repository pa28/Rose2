//
// Created by richard on 01/01/23.
//

/*
 * GraphicsModel.cpp Created by Richard Buckley (C) 01/01/23
 */

#include <SDL.h>
#include <SDL_ttf.h>

#include "GraphicsModel.h"
#include "Utilities.h"

namespace rose::gm {

    /**
     * @class Fps
     * @brief Throttle the application to limit the number of frames per second the scene is refreshed.
     */
    class [[maybe_unused]] Fps {
    public:
        /**
         * @brief Constructor
         * @param tickInterval The number of SDL ticks desired between frames.
         */
        explicit Fps(Uint32 tickInterval = 30)
                : m_tickInterval(tickInterval), m_nextTime(SDL_GetTicks() + tickInterval) {
        }

        /**
         * @brief Wait until the next frame interval.
         */
        [[maybe_unused]] void next() {
            SDL_Delay(getTicksToNextFrame());

            m_nextTime += m_tickInterval;
        }

    private:
        Uint32 m_tickInterval;       ///< The number of SDL ticks per frame.
        Uint32 m_nextTime;              ///< The time of the next frame start.

        /**
         * Compute the number of ticks to wait until the next frame time.
         * @return The number of ticks to wait
         */
        [[nodiscard]] Uint32 getTicksToNextFrame() const {
            Uint32 now = SDL_GetTicks();

            return (m_nextTime <= now) ? 0 : m_nextTime - now;
        }
    };

    /**
     * Context
     */

    [[maybe_unused]] void Context::copyFullTexture(Texture &source, Texture &destination) {
        RenderTargetGuard renderTargetGuard(*this, destination);
        renderCopy(source);
        destination.setBlendMode(SDL_BLENDMODE_BLEND);
    }

    int Context::renderCopy(const Texture &texture) {
        if (!texture) {
            std::cerr << __PRETTY_FUNCTION__ << "Invalid Texture.\n";
            return -1;
        }
        return SDL_RenderCopy(mRenderer.get(), texture.get(), nullptr, nullptr);
    }

    [[maybe_unused]] int Context::renderCopy(const Texture &texture, Rectangle dst) {
        SDL_Rect dstRect{dst.point.x, dst.point.y, dst.size.w, dst.size.h};
        if (!texture) {
            std::cerr << __PRETTY_FUNCTION__ << "Invalid Texture.\n";
            return -1;
        }

        return SDL_RenderCopy(mRenderer.get(), texture.get(), nullptr, &dstRect);
    }

    [[maybe_unused]] int Context::renderCopy(const Texture &texture, Rectangle src, Rectangle dst) {
        SDL_Rect srcRect{src.point.x, src.point.y, src.size.w, src.size.h};
        SDL_Rect dstRect{dst.point.x, dst.point.y, src.size.w, src.size.h};
        if (!texture) {
            std::cerr << __PRETTY_FUNCTION__ << "Invalid Texture.\n";
            return -1;
        }
        return SDL_RenderCopy(mRenderer.get(), texture.get(), &srcRect, &dstRect);
    }

    [[maybe_unused]] int Context::renderCopyEx(Texture &texture, Rectangle src, Rectangle dst, double angle, RenderFlip renderFlip,
                              std::optional<Point> point) const {
        SDL_Rect srcRect{src.point.x, src.point.y, src.size.w, src.size.h};
        SDL_Rect dstRect{dst.point.x, dst.point.y, dst.size.w, dst.size.h};
        if (point) {
            SDL_Point sdlPoint;
            sdlPoint.x = point->x;
            sdlPoint.y = point->y;
            return SDL_RenderCopyEx(get(), texture.get(), &srcRect, &dstRect, angle, &sdlPoint, renderFlip.mFlip);
        } else {
            return SDL_RenderCopyEx(get(), texture.get(), &srcRect, &dstRect, angle, nullptr, renderFlip.mFlip);
        }
    }

    [[maybe_unused]] int Context::drawLine(const Point &p0, const Point &p1) const {
        return SDL_RenderDrawLine(get(), p0.x, p0.y, p1.x, p1.y);
    }

    /**
     * RenderTargetGuard
     */
    RenderTargetGuard::~RenderTargetGuard() noexcept(false) {
        mContext.mCurrentRenderTarget = mLastTexture;
        status = SDL_SetRenderTarget(mContext.get(), mContext.mCurrentRenderTarget);
    }

    RenderTargetGuard::RenderTargetGuard(Context &context, Texture &texture) : mContext(context) {
        mLastTexture = context.mCurrentRenderTarget;
        context.mCurrentRenderTarget = texture.get();
        status = SDL_SetRenderTarget(context.get(), context.mCurrentRenderTarget);
    }

    [[maybe_unused]] int RenderTargetGuard::setRenderTarget(Texture &texture) {
        mContext.mCurrentRenderTarget = texture.get();
        return SDL_SetRenderTarget(mContext.get(), mContext.mCurrentRenderTarget);
    }

    /**
     * Texture
     */

    [[maybe_unused]] Texture::Texture(Context &context, SDL_PixelFormatEnum format, SDL_TextureAccess access, int width, int height) {
        reset(SDL_CreateTexture(context.get(), format, access, width, height));
        if (!operator bool()) {
            throw TextureRuntimeError(
                    StringCompositor("SDL_CreateTexture: (", width, 'x', height, ") -- ",
                                     SDL_GetError()));
        }
    }

    [[maybe_unused]] Texture::Texture(Context &context, Size size) {
        reset(SDL_CreateTexture(context.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.w, size.h));
        if (!operator bool()) {
            throw TextureRuntimeError(
                    StringCompositor("SDL_CreateTexture: (", size.w, 'x', size.h, ") -- ",
                                     SDL_GetError()));
        }
    }

    [[maybe_unused]] int Texture::setAlphaMod(float alpha) {
        uint8_t alphaMod = static_cast<uint8_t>(255.f * std::clamp(alpha, 0.f, 1.f));
        return SDL_SetTextureAlphaMod(get(), alphaMod);
    }

    /**
     * GraphicsModel
     */
    bool GraphicsModel::initialize(const std::string &title, Size initialSize, const Point &initialPosition,
                                   uint32_t extraFlags) {
//        Settings &settings{Settings::getSettings()};
//        SDL_RendererInfo info;

        // Set image scaling quality to the highest value available
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);   // Initialize SDL2
        TTF_Init();

        atexit(SDL_Quit);

//        SDL_Window *window;        // Declare a pointer to an SDL_Window
        uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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
            std::cout << "    Number of displays: " << SDL_GetNumVideoDisplays() << ", On: "
                      << SDL_GetWindowDisplayIndex(mSdlWindow.get()) << '\n'
                      << "    Version " << (int) sdlVersion.major << '.' << (int) sdlVersion.minor << '.'
                      << (int) sdlVersion.patch << '\n';
            for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i) {
                SDL_Rect displayBounds{0, 0, 0, 0};

                if (SDL_GetDisplayBounds(i, &displayBounds)) {
                    mDisplayBounds.emplace_back();
                } else {
                    mDisplayBounds.emplace_back(
                            displayBounds.x, displayBounds.y, displayBounds.w, displayBounds.h);
                    std::cout << "    Display " << i << ": " << mDisplayBounds.back() << '\n';
                }
            }

            mContext = Context{mSdlWindow, -1, RendererFlags::RENDERER_ACCELERATED
                                               | RendererFlags::RENDERER_TARGETTEXTURE
                                               | RendererFlags::RENDERER_PRESENTVSYNC};

            if (mContext) {
                mContext.setDrawBlendMode(SDL_BLENDMODE_BLEND);
            } else {
                ErrorCode = RoseErrorCode::SDL_RENDERER_CREATE;
                std::cerr << "Could not create SDL_Renderer: " << SDL_GetError() << '\n';
                return false;
            }
        } else {
            std::string sdlError{SDL_GetError()};
            std::cerr << "Could not create SDL_Window: " << sdlError << '\n';
            if (sdlError == "Could not initialize EGL") {
                throw std::runtime_error("If X11 is running, check DISPLAY environment variable.");
            }
            ErrorCode = RoseErrorCode::SDL_WINDOW_CREATE;
            return false;
        }
        return true;
    }

#if 0
    void GraphicsModel::eventLoop(std::shared_ptr<Screen> &screen) {
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
                if (eventCallback)
                    eventCallback(e);
            }

//            drawAll(screen);

            fps.next();
        }
    }

    void GraphicsModel::drawAll(std::shared_ptr<Screen> &screen) {
        CommonSignals::getCommonSignals().frameSignal.transmit(mFrame);

        if (mRedrawBackground) {
            screen->erase(std::remove_if(screen->begin(), screen->end(), [&](auto ref)->bool {
                if (auto popup = std::dynamic_pointer_cast<PopupWindow>(ref); popup) {
                    return popup->removePopup();
                }
                return false;
            }),screen->end());

            for (auto &content : *screen) {
                if (auto window = std::dynamic_pointer_cast<Window>(content); window) {
                    window->generateBaseTexture(mContext, Position<int>{});
                }
            }
        }

        if (Animator::getAnimator() || mRedrawBackground) {
            mContext.renderClear();
            for (auto & content : *screen) {
                if (auto window = std::dynamic_pointer_cast<Window>(content); window) {
                    if (window->baseTextureNeeded(Position<int>{}))
                        window->generateBaseTexture(mContext, Position<int>{});
                    window->drawBaseTexture(mContext, Position<int>{});

                    Animator::getAnimator().animate(window, mContext, mFrame);
                }
            }
            mContext.renderPresent();
        }

        mRedrawBackground = false;
        mFrame++;
    }
#endif
} // rose::gm
