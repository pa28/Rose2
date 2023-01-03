//
// Created by richard on 02/01/23.
//

/*
 * Color.h Created by Richard Buckley (C) 02/01/23
 */

#ifndef ROSE2_COLOR_H
#define ROSE2_COLOR_H

#include <SDL.h>
#include <array>
#include <algorithm>

namespace rose {
/**
 * @struct Color
 */
    struct Color {
        constexpr static size_t RED = 0;
        constexpr static size_t GREEN = 1;
        constexpr static size_t BLUE = 2;
        constexpr static size_t ALPHA = 3;

        /**
         * Red, Green, Blue and Alpha channels as floats
         */
        std::array<float,4> channels{};

        Color() = default;
        Color(const Color&) = default;
        Color(Color&&) = default;
        Color& operator=(const Color&) = default;
        Color& operator=(Color&&) = default;
        ~Color() = default;

        template<class T>
        requires std::is_floating_point_v<T>
        constexpr Color(T r, T g, T b, T a) :
            channels({(float)r,(float)g,(float)b,(float)a}) {
            std::ranges::transform( channels.begin(), channels.begin(), channels.begin(),
                                    [] (auto chan) { return fmin(1.0, fabs(chan)); });
        }

        template<class T>
        requires std::is_integral_v<T>
        constexpr Color(T r, T g, T b, T a) {
            channels[RED] = static_cast<float>(r) / 255.0f;
            channels[GREEN] = static_cast<float>(g) / 255.0f;
            channels[BLUE] = static_cast<float>(b) / 255.0f;
            channels[ALPHA] = static_cast<float>(a) / 255.0f;
        }

        [[maybe_unused]] [[nodiscard]] constexpr SDL_Color sdlColor() const {
            std::array<Uint8,4> sdlChan{};
            std::ranges::transform(channels.begin(), channels.end(), sdlChan.begin(),
                                   [] (auto chan) {
                return static_cast<Uint8>(255. * fabs(chan));
            });
            return SDL_Color{sdlChan[RED], sdlChan[GREEN], sdlChan[BLUE], sdlChan[ALPHA]};
        }

        auto operator <=> (const Color &) const = default;
    };

    namespace color {
        [[maybe_unused]] constexpr Color TransparentBlack{0.0, 0.0, 0.0, 0.0};
        [[maybe_unused]] constexpr Color OpaqueBlack{0.0, 0.0, 0.0, 1.0};
        [[maybe_unused]] constexpr Color OpaqueWhite{1.0, 1.0, 1.0, 1.0};
    }

} // rose


#endif //ROSE2_COLOR_H
