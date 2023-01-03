//
// Created by richard on 03/01/23.
//

/*
 * Surface.cpp Created by Richard Buckley (C) 03/01/23
 */

/**
 * @file Surface.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 03/01/23
 */

#include <SDL_image.h>
#include "Surface.h"

namespace rose {

    Surface::Surface(const std::filesystem::path &path) : Surface() {
        reset(IMG_Load(path.c_str()));
        if (!operator bool()) {
            throw SurfaceRuntimeError(StringCompositor("IMG_Load from: ", path.string(), " -- ", IMG_GetError()));
        }
    }

    Surface::Surface(int width, int height, int depth, SDL_PixelFormatEnum format) : Surface() {
        reset(SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format));
        if (!operator bool()) {
            throw SurfaceRuntimeError(
                    StringCompositor("SDL_CreateRGBSurfaceWithFormat: (", width, 'x', height, ") -- ",
                                     SDL_GetError()));
        }
    }

    Surface::Surface(int width, int height, int depth, uint32_t rmask, uint32_t gmask, uint32_t bmask, uint32_t amask) : Surface() {
        reset(SDL_CreateRGBSurface(0, width, height, depth, rmask, gmask, bmask, amask));
        if (!operator bool()) {
            throw SurfaceRuntimeError(
                    StringCompositor("SDL_CreateRGBSurface: (", width, 'x', height, ") -- ", SDL_GetError()));
        }
    }

    uint32_t &Surface::pixel(int x, int y) const {
        auto *pixels = (Uint32 *) get()->pixels;
        return pixels[(y * get()->w) + x];
    }

    Color Surface::color(int x, int y) const {
        auto p = pixel(x, y);
        uint8_t r, g, b, a;
        SDL_GetRGBA(p, get()->format, &r, &g, &b, &a);
        return Color{r, g, b, a};
    }

    void Surface::setColor(int x, int y, Color color) {
        auto c = color.sdlColor();
        pixel(x, y) = SDL_MapRGBA(get()->format, c.r, c.g, c.g, c.a);
    }

    bool Surface::createWithFormat(int width, int height, int depth, SDL_PixelFormatEnum format) {
        reset(SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format));
        if (!operator bool()) {
            throw SurfaceRuntimeError(
                    StringCompositor("SDL_CreateRGBSurfaceWithFormat: (", width, 'x', height, ") -- ",
                                     SDL_GetError()));
        }
        return operator bool();
    }

    int Surface::fillRectangle(const Rectangle &rect, const Color &color) {
        auto c = color.sdlColor();
        SDL_Rect r{rect.point.x, rect.point.y, rect.size.w, rect.size.h};
        return SDL_FillRect(get(), &r, SDL_MapRGBA(get()->format, c.r, c.g, c.b, c.a));
    }

    int Surface::fillRectangle(const Color &color) {
        auto c = color.sdlColor();
        return SDL_FillRect(get(), nullptr, SDL_MapRGBA(get()->format, c.r, c.g, c.b, c.a));
    }

    bool Surface::textureFromSurface(Context &context, Texture &texture) {
        texture.reset(SDL_CreateTextureFromSurface(context.get(), get()));
        if (!texture.operator bool())
            throw SurfaceRuntimeError(StringCompositor("SDL_CreateTextureFromSurface: ", SDL_GetError()));
        return texture.operator bool();
    }

    Texture Surface::toTexture(Context &context) {
        Texture texture{};
        texture.reset(SDL_CreateTextureFromSurface(context.get(), get()));
        if (!texture) {
            std::cerr << __PRETTY_FUNCTION__ << " Error: " << SDL_GetError() << '\n';
        }
        return texture;
    }

    int Surface::setBlendMode(SDL_BlendMode blendMode) noexcept {
        return SDL_SetSurfaceBlendMode(get(), blendMode);
    }

    int Surface::blitSurface(Surface &source) {
        return SDL_BlitSurface(source.get(), nullptr, get(), nullptr);
    }

    SurfaceLock::~SurfaceLock() {
        SDL_UnlockSurface(mSurface);
    }

    SurfaceLock::SurfaceLock(SDL_Surface *surface) : mSurface(surface) {
        status = SDL_LockSurface(mSurface);
    }
} // rose