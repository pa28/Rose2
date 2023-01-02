//
// Created by richard on 01/01/23.
//

/*
 * GraphicsModel.cpp Created by Richard Buckley (C) 01/01/23
 */

#include <SDL.h>
#include <SDL_ttf.h>

#include <GraphicsModel.h>
#include <Color.h>

#include <fmt/format.h>

namespace rose {

    /**
     * Context
     */

    [[maybe_unused]] void Context::copyFullTexture(Texture &source, Texture &destination) {
        RenderTargetGuard renderTargetGuard(*this, destination);
        renderCopy(source);
        destination.setBlendMode(SDL_BLENDMODE_BLEND);
    }

    void Context::renderCopy(const Texture &texture) {
        if (!texture) {
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, "Invalid Texture"));
        }
        if (SDL_RenderCopy(mRenderer.get(), texture.get(), nullptr, nullptr))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
    }

    [[maybe_unused]] void Context::renderCopy(const Texture &texture, Rectangle dst) {
        SDL_Rect dstRect{dst.point.x, dst.point.y, dst.size.w, dst.size.h};
        if (!texture) {
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, "Invalid Texture"));
        }

        if (SDL_RenderCopy(mRenderer.get(), texture.get(), nullptr, &dstRect))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
    }

    [[maybe_unused]] void Context::renderCopy(const Texture &texture, Rectangle src, Rectangle dst) {
        SDL_Rect srcRect{src.point.x, src.point.y, src.size.w, src.size.h};
        SDL_Rect dstRect{dst.point.x, dst.point.y, src.size.w, src.size.h};
        if (!texture) {
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, "Invalid Texture"));
        }
        if (SDL_RenderCopy(mRenderer.get(), texture.get(), &srcRect, &dstRect))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
    }

    [[maybe_unused]] void Context::renderCopyEx(Texture &texture, Rectangle src, Rectangle dst, double angle, RenderFlip renderFlip,
                                                std::optional<Point> point) const {
        SDL_Rect srcRect{src.point.x, src.point.y, src.size.w, src.size.h};
        SDL_Rect dstRect{dst.point.x, dst.point.y, dst.size.w, dst.size.h};
        if (point) {
            SDL_Point sdlPoint;
            sdlPoint.x = point->x;
            sdlPoint.y = point->y;
            if (SDL_RenderCopyEx(get(), texture.get(), &srcRect, &dstRect, angle, &sdlPoint, renderFlip.mFlip))
                throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
        } else {
            if (SDL_RenderCopyEx(get(), texture.get(), &srcRect, &dstRect, angle, nullptr, renderFlip.mFlip))
                throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
        }
    }

    [[maybe_unused]] void Context::setDrawColor(Color color) const {
        auto sdlColor = color.sdlColor();
        if (SDL_SetRenderDrawColor(get(), sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
    }

    void Context::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
        if (SDL_SetRenderDrawColor(get(), r, g, b, a))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
    }

    [[maybe_unused]] void Context::drawLine(const Point &p0, const Point &p1) const {
        if (SDL_RenderDrawLine(get(), p0.x, p0.y, p1.x, p1.y))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
    }

    void Context::fillRect(Rectangle rect) const {
        SDL_Rect sdlRect;
        sdlRect.h = rect.size.h;
        sdlRect.w = rect.size.w;
        sdlRect.x = rect.point.x;
        sdlRect.y = rect.point.y;
        if (SDL_RenderFillRect(get(), &sdlRect))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
    }

    [[maybe_unused]] void Context::drawPoint(const Point &p) const {
        if (SDL_RenderDrawPoint(get(), p.x, p.y))
            throw ContextException(fmt::format("{}: {}", __FUNCTION__, SDL_GetError()));
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
            throw TextureRuntimeError(fmt::format("SDL_CreateTexture: ({}x{}) -- {}", width, height, SDL_GetError()));
        }
    }

    [[maybe_unused]] Texture::Texture(Context &context, Size size) {
        reset(SDL_CreateTexture(context.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.w, size.h));
        if (!operator bool()) {
            throw TextureRuntimeError(fmt::format("SDL_CreateTexture: ({}x{}) -- {}", size.w, size.h, SDL_GetError()));
        }
    }

    [[maybe_unused]] int Texture::setAlphaMod(float alpha) {
        uint8_t alphaMod = static_cast<uint8_t>(255.f * std::clamp(alpha, 0.f, 1.f));
        return SDL_SetTextureAlphaMod(get(), alphaMod);
    }

    /**
     * GraphicsModel
     */
    bool GraphicsModel::initialize() {
//        Settings &settings{Settings::getSettings()};
//        SDL_RendererInfo info;

        // Set image scaling quality to the highest value available
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);   // Initialize SDL2
        TTF_Init();

        atexit(SDL_Quit);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        return true;
    }

    void GraphicsModel::eventLoop() {
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

    DrawColorGuard::DrawColorGuard(Context &context, SDL_Color color) : mContext(context) {
        mStatus = 0;
        if (int status = SDL_GetRenderDrawColor( mContext.get(), &mOldColor.r, &mOldColor.g,
                                                 &mOldColor.b, &mOldColor.a); status == 0 ) {
            mStatus = SDL_SetRenderDrawColor( mContext.get(), color.r, color.g, color.b, color.a);
        } else {
            mStatus = status;
        }
    }
} // rose
