//
// Created by richard on 01/01/23.
//

/**
 * @file GraphicsModel.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 2.0
 * @date 2023-01-01
 * @brief Abstraction of the graphics model.
 */

#ifndef ROSE2_GRAPHICSMODEL_H
#define ROSE2_GRAPHICSMODEL_H

#include <memory>
#include <vector>
#include <optional>
#include <functional>
#include <SDL.h>
#include <Rose.h>
#include "Color.h"
#include "Utilities.h"
#include "fmt/format.h"

namespace rose {
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
     * @brief A functor to destroy an SDL_Window in a std::unique_ptr (rose::sdl::Window).
     */
    class SdlWindowDestroy {
    public:
        /**
         * @brief Call the SDL API to destroy an SDL_Window.
         * @param sdlWindow A pointer to the SDL_Window to destroy.
         */
        void operator()(SDL_Window *sdlWindow) {
            SDL_DestroyWindow(sdlWindow);
        }
    };

    using SdlWindow = std::unique_ptr<SDL_Window, SdlWindowDestroy>;   //!< An SDL_Window unique pointer

    /**
     * @struct RenderFlip
     * @brief A Widget manipulator to indicate if and how rendering a texture should be flipped.
     * @details The structure supports flipping on the Horizontal, Vertical or both axis.
     */
    struct RenderFlip {
        SDL_RendererFlip mFlip;

        /// Default constructor -- No flipping
        constexpr RenderFlip() noexcept: mFlip(SDL_FLIP_NONE) {}

        /// Constructor -- user specified flipping.
        [[maybe_unused]] constexpr explicit RenderFlip(SDL_RendererFlip flip) noexcept: mFlip(flip) {}
    };


    /**
     * @brief A functor to destroy an SDL_Texture in a std::unique_ptr (rose::sdl::Texture)
     */
    class TextureDestroy {
    public:
        /**
         * @brief Call the SDL API to destroy an SDL_Texture.
         * @param sdlTexture A pointer to the SDL_Texture to destroy.
         */
        void operator()(SDL_Texture *sdlTexture) {
            if (sdlTexture != nullptr)
                SDL_DestroyTexture(sdlTexture);
        }
    };

    class TextureRuntimeError : public std::runtime_error {
    public:
        ~TextureRuntimeError() override = default;

        explicit TextureRuntimeError(const std::string &what) : std::runtime_error(what) {}

        [[maybe_unused]] explicit TextureRuntimeError(const char *what) : std::runtime_error(what) {}
    };

    class Context;

    /**
     * @class Texture
     * @brief Abstraction of SDL_Texture
     */
    class Texture : public std::unique_ptr<SDL_Texture,TextureDestroy> {
    public:
        Texture() = default;

        Texture(const Texture&) = delete;
        Texture(Texture &&) = default;
        Texture& operator=(const Texture &) = delete;
        Texture& operator=(Texture &&) = default;

        /**
         * @breif Create a Texture
         * @param context The Context to use.
         * @param format The pixel format from SDL_PixelFormatEnum.
         * @param access The Texture access from SDL_TextureAccess.
         * @param width The width of the texture.
         * @param height The height of the texture.
         */
        [[maybe_unused]] Texture(Context& context, SDL_PixelFormatEnum format, SDL_TextureAccess access, int width, int height);

        /**
         * @brief Create a Texture
         * @details Builds a Texture compatible with building up textures within Rose . The pixel format is
         * SDL_PIXELFORMAT_RGBA8888, the texture access is SDL_TEXTUREACCESS_TARGET.
         * @param context The renderer to use.
         * @param size The size of the texture.
         */
        [[maybe_unused]] Texture(Context &context, Size size);

        int setBlendMode(SDL_BlendMode blendMode) {
            return SDL_SetTextureBlendMode(get(), blendMode);
        }

        [[maybe_unused]] [[nodiscard]] Size getSize() const {
            Size size{};
            SDL_QueryTexture(get(), nullptr, nullptr, &size.w, &size.h);
            return size;
        }

        [[maybe_unused]] int setAlphaMod(float alpha);
    };

    /**
     *  @brief Flags used when creating a rendering context
     */
    enum RendererFlags : uint32_t {
        RENDERER_SOFTWARE [[maybe_unused]] = static_cast<uint32_t>(SDL_RENDERER_SOFTWARE),         /**< The renderer is a software fallback */
        RENDERER_ACCELERATED = static_cast<uint32_t>(SDL_RENDERER_ACCELERATED),   /**< The renderer uses hardware acceleration */
        RENDERER_PRESENTVSYNC = static_cast<uint32_t>(SDL_RENDERER_PRESENTVSYNC), /**< Present is synchronized with the refresh rate */
        RENDERER_TARGETTEXTURE = static_cast<uint32_t>(SDL_RENDERER_TARGETTEXTURE) /**< The renderer supports rendering to texture */
    };

    /**
     * @classs Context
     * @brief An abstraction of graphics rendering context.
     */
    class Context {
        friend class RenderTargetGuard;

    protected:
        /**
         * @brief A functor to destroy an SDL_Renderer
         */
        class RendererDestroy {
        public:
            /**
             * @brief Call the SDL API to destroy the renderer.
             * @param sdlRenderer The SDL_Renderer
             */
            void operator()(SDL_Renderer *sdlRenderer) {
                SDL_DestroyRenderer(sdlRenderer);
            }
        };

        using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDestroy>; ///< An SDL_Renderer unique pointer
        RendererPtr mRenderer{};    ///< The Renderer.

        SDL_Texture *mCurrentRenderTarget{nullptr};

    public:

        Context() = default;

        Context(const Context &context) = delete;

        Context(Context &&context) noexcept = default;

        Context &operator=(const Context &context) = delete;

        Context &operator=(Context &&context) = default;

        explicit Context(SdlWindow &window, int index, Uint32 flags) : Context()
        { mRenderer.reset(SDL_CreateRenderer(window.get(), index, flags)); }

        /// Test for a valid Context
        explicit operator bool() const noexcept { return mRenderer.operator bool(); }

        /// Get an opaque pointer for API calls.
        [[nodiscard]] auto get() const { return mRenderer.get(); }

        /// Set the draw blend mode.
        void setDrawBlendMode(SDL_BlendMode blendMode) {
            SDL_SetRenderDrawBlendMode(mRenderer.get(), blendMode);
        }

        /**
         * @brief Copy source Texture to destination Texture and set the BlendMode on the destination Texture.
         * @details The function uses RenderTargetGuard to temporarily set the render Target to the destination,
         * calls SDL_RenderCopy to copy the texture, and sets the BlendMode on the destination texture to
         * SDL_BLENDMODE_BLEND.
         * @param source
         * @param destination
         */
        [[maybe_unused]] void copyFullTexture(Texture &source, Texture &destination);

        /// Prepare for the start of a rendering iteration.
        int renderClear() { return SDL_RenderClear(mRenderer.get()); }

        /// Complete a rendering iteration.
        void renderPresent() { SDL_RenderPresent(mRenderer.get()); }

        /**
         * @brief Copy a Texture to the current render target using the size of the Texture and the size of the
         * target.
         * @param texture The texture to copy.
         * @return The return status of the API call.
         */
        void renderCopy(const Texture &texture);

        /**
         * @brief Copy a Texture to the current render target using the size of the Texture and specified
         * destination rectangle.
         * @param texture The source Texture.
         * @param dst The destination Rectangle.
         * @return The return status of the API call.
         */
        [[maybe_unused]] void renderCopy(const Texture &texture, Rectangle dst);

        /**
         * @brief Copy part of the Texture specified by the source Rectangle to the render target location
         * specified by the destination Rectangle.
         * @param texture The Texture.
         * @param src The source Rectangle.
         * @param dst The destination Rectangle.
         * @return The return status of API call.
         */
        [[maybe_unused]] void renderCopy(const Texture &texture, Rectangle src, Rectangle dst);

        /**
         * @brief Render with extensions.
         * @param src Source Rectangle.
         * @param dst Destination Rectangle.
         * @param angle Rotation Angle.
         * @param renderFlip Flip
         * @param point Rotation Position if specified.
         * @return Status code returned by SDL_RenderCopyEx()
         */
        [[maybe_unused]] void renderCopyEx(Texture &texture, Rectangle src, Rectangle dst, double angle, RenderFlip renderFlip,
                                           std::optional<Point> point = std::nullopt) const;

        /**
         * @brief Set the drawing color used for drawing Rectangles, lines and clearing.
         * @param color The new drawing Color.
         * @return Status code returned by the API.
         */
        [[maybe_unused]] [[nodiscard]] void setDrawColor(Color color) const;

        /**
         * @brief Set the drawing color used or drawing Rectangles, lines and clearing.
         * @param r red channel
         * @param g green channel
         * @param b blue channel
         * @param a alpha channel
         * @return Status code returned by the API.
         */
        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;

        /**
         * @brief Set the drawing color used for drawing Rectangles, lines and clearing.
         * @param color The new drawing Color.
         * @return Status code returned by the API.
         */
//        int setDrawColor(color::HSVA color) {
//            return setDrawColor(color.toRGBA());
//        }

        /**
         * @brief Render a filled Rectangle.
         * @param rect The Rectangle
         * @return The status return from the SDL API.
         */
        [[nodiscard]] void fillRect(Rectangle rect) const;

        /**
         * @brief Render a pixel.
         * @param p The location of the pixel.
         * @return The status return from the SDL API.
         */
        [[maybe_unused]] [[nodiscard]] void drawPoint(const Point &p) const;

        /**
         * @brief Render a line.
         * @param p0 Start of the line.
         * @param p1 End of the line
         * @return The status return from the SDL API.
         */
        [[maybe_unused]] [[maybe_unused]] void drawLine(const Point &p0, const Point &p1) const;
    };

    /**
     * @class RenderTargetGuard
     * @brief Store the current render target replacing it with a new render target. When the object is
     * destroyed (by going out of scope) the old render target is restored.
     */
    class RenderTargetGuard {
    protected:
        Context &mContext;                      ///< The Context being guarded
//        bool popped{false};                     ///< True if the target guard has already been popped off the stack.
        SDL_Texture *mLastTexture{nullptr};     ///< Save the current render target here.
        int status{0};                          ///< The return status from the last SDL API call.

    public:
        RenderTargetGuard() = delete;   ///< Deleted default constructor.
        RenderTargetGuard(const RenderTargetGuard &) = delete;   ///< Deleted copy constructor.

        /**
         * @brief Set the old render target back on the renderer when destroyed.
         */
        ~RenderTargetGuard() noexcept(false);

        /**
         * @brief Test the status of the RenderTargetGuard.
         * @details The status is good (true is returned) if the status value returned from the last
         * operation on the Context object returned 0.
         * @return True if the last operation succeeded.
         */
        explicit operator bool() const noexcept { return status == 0; }

        /**
         * @brief Constructor
         * @param renderer The renderer which render target will be managed.
         * @param texture The texture which will become the new render target.
         */
        RenderTargetGuard(Context &context, Texture &texture);

        /**
         * @brief Set a new render target without pushing it on the stack.
         * @details This may be used when a number of render target changes are needed in a context block.
         * A RenderTargetGuard is created, calls to setRenderTarget are used to manipulate the render target.
         * When the RenderTargetGuard goes out of scope the original render target is restored.
         * @param texture
         * @return
         */
        [[maybe_unused]] int setRenderTarget(Texture &texture);
    };

    /**
     * @class DrawColorGuardException
     * @brief Thrown by DrawColorGuard on errors.
     */
    class DrawColorGuardException : public std::runtime_error {
    public:
        explicit DrawColorGuardException(const std::string &what_arg) : std::runtime_error(what_arg) {}

        [[maybe_unused]] explicit DrawColorGuardException(const char *what_arg) : std::runtime_error(what_arg) {}
    };

    /**
     * @class ContextException
     * @brief Thrown by SDL API errors in Context methods.
     */
    class ContextException : public std::runtime_error {
    public:
        explicit ContextException(const std::string &what_arg) : std::runtime_error(what_arg) {}

        [[maybe_unused]] explicit ContextException(const char *what_arg) : std::runtime_error(what_arg) {}
    };

    /**
     * @class DrawColorGuard
     * @brief Store the current draw color replacing it with a new draw color. When the object is
     * destroyed (by going out of scope) the old draw color is set.
     */
    class [[maybe_unused]] DrawColorGuard {
    protected:
        Context &mContext;      ///< The renderer to which the draw colors are set.
        SDL_Color mOldColor{};  ///< The old draw color
        int mStatus;            ///< The status of the last SDL operation.

    public:
        DrawColorGuard() = delete;  ///< Deleted default constructor.
        DrawColorGuard(const DrawColorGuard &) = delete; ///< Deleted copy constructor.
        DrawColorGuard(DrawColorGuard &&) = default;

        DrawColorGuard &operator=(const DrawColorGuard &) = delete;

        /**
         * @brief Set the old clip rectangle back on the renderer when destroyed.
         */
        ~DrawColorGuard() noexcept(false) {
            if (SDL_SetRenderDrawColor(mContext.get(), mOldColor.r, mOldColor.g, mOldColor.b, mOldColor.a)) {
                throw DrawColorGuardException(StringCompositor("Call to SDL_XxxRenderDrawColor failed:",
                                                               SDL_GetError()));
            }
        }

        /**
         * @brief Construct DrawColorGuard with and SDL_Color.
         * @param renderer The renderer to set the color on.
         * @param color The SDL_Color.
         */
        DrawColorGuard(Context &context, SDL_Color color);

        /**
         * @brief Construct DrawColorGuard with a rose::Color.
         * @param renderer The renderer to set the color on.
         * @param color The rose::Color.
         */
        [[maybe_unused]] DrawColorGuard(Context &context, Color& color) : DrawColorGuard(context, color.sdlColor()) {}

        /**
         * @brief Test the validity of the DrawColorGuard.
         * @return False if the last SDL operation return an error.
         */
        explicit operator bool() const noexcept { return mStatus == 0; }

        /**
         * @brief Set the draw Color on the renderer without pushing the old color on the stack.
         * @param color The SDL_Color to set.
         * @return The SDL2 API return status code.
         */
        int setDrawColor(SDL_Color color) {
            return SDL_SetRenderDrawColor(mContext.get(), color.r, color.g, color.b, color.a);
        }

        /**
         * @brief Set the draw Color on the renderer without pushing the old color on the stack.
         * @param color The Color to set.
         * @return The SDL2 API return status code.
         */
        [[maybe_unused]] void setDrawColor(Color& color) {
            if (setDrawColor(color.sdlColor()))
                throw DrawColorGuardException(fmt::format("DrawColorGuard exception {}: {}",
                                                          __FUNCTION__, SDL_GetError()));
        }
    };

    /**
     * @class ClipRectangleGuard
     * @brief Store the current clip rectangle replacing it with a new clip rectangle. When the object is
     * destroyed (by going out of scope) the old clip rectangle is set.
     */
    class [[maybe_unused]] ClipRectangleGuard {
    protected:
        Context &mContext;    ///< The renderer to which the clip rectangles are set.
        SDL_Rect mOldClip{};    ///< The old clip rectangle
        int mStatus{0};

    public:
        ClipRectangleGuard() = delete;  ///< Deleted default constructor
        ClipRectangleGuard(const ClipRectangleGuard &) = delete;     ///< Deleted copy constructor

        explicit operator bool () const { return mStatus == 0; }

        /**
         * @brief Set the old clip rectangle back on the renderer when destroyed.
         */
        ~ClipRectangleGuard() {
            if (mOldClip.w == 0 && mOldClip.y == 0)
                mStatus = SDL_RenderSetClipRect(mContext.get(), nullptr);
            else
                mStatus = SDL_RenderSetClipRect(mContext.get(), &mOldClip);
        }

        /**
         * @brief Speculative constructor.
         * @details This constructor saves the current clip rectangle to be restored later, but does not
         * set a new clip rectangle. A new clip rectangle may be set using the assignment operators.
         * @param context The renderer to guard the clip rectangle of.
         */
        [[maybe_unused]] explicit ClipRectangleGuard(Context &context) : mContext(context) {
            SDL_RenderGetClipRect(mContext.get(), &mOldClip);
        }

        /**
         * @brief Constructor. Store the current clip rectangle and set the new one.
         * @param context The renderer to set the clip rectangles on.
         * @param clip The new clip rectangle.
         */
        ClipRectangleGuard(Context &context, const SDL_Rect &clip) : mContext(context) {
            SDL_RenderGetClipRect(mContext.get(), &mOldClip);
            mStatus = SDL_RenderSetClipRect(mContext.get(), &clip);
        }

        /**
         * @brief Constructor. Store the current clip rectangle and set the new one.
         * @param renderer The renderer to set the clip rectangles on.
         * @param x X co-ordinate of the new clip rectangle.
         * @param y Y co-ordinate of the new clip rectangle.
         * @param w Width of the new clip rectangle.
         * @param h Height of the new clip rectangle.
         */
        [[maybe_unused]] ClipRectangleGuard(Context &context, int x, int y, int w, int h)
                : ClipRectangleGuard(context, SDL_Rect{x, y, w, h}) {}

        /**
         * @brief Conditional constructor.
         * @details This constructor saves the current clip rectangle to be restored later. If clip
         * is valid it is converted to an SDL_Rect and set as the new clip rectangle
         * @param renderer The renderer to set the clip rectangles on.
         * @param clip A, possibly invalid, RectangleInt.
         */
        [[maybe_unused]] ClipRectangleGuard(Context &context, const Rectangle &clip) : mContext(context) {
            SDL_RenderGetClipRect(mContext.get(), &mOldClip);
            SDL_Rect rect{clip.point.x, clip.point.y, clip.size.w, clip.size.h};
            mStatus = SDL_RenderSetClipRect(mContext.get(), &rect);
        }

        /**
         * @brief Assign a new clip rectangle through the ClipRectangleGuard.
         * @param clip The new clip rectangle.
         * @return The ClipRectangleGuard.
         */
        ClipRectangleGuard &operator=(SDL_Rect &clip) {
            mStatus = SDL_RenderSetClipRect(mContext.get(), &clip);
            return *this;
        }

        /**
         * @brief Assign a new clip rectangle through the ClipRectangleGuard.
         * @details If clip is invalid no changes are made.
         * @param clip The new clip rectangle.
         * @return The ClipRectangleGuard.
         */
        ClipRectangleGuard &operator=(Rectangle &clip) {
            SDL_Rect rect{clip.point.x, clip.point.y, clip.size.w, clip.size.h};
            mStatus = SDL_RenderSetClipRect(mContext.get(), &rect);
            return *this;
        }

        [[maybe_unused]] ClipRectangleGuard &intersection(Rectangle &clip) {
            SDL_Rect current;
            SDL_RenderGetClipRect(mContext.get(), &current);
            if (SDL_RectEmpty(&current)) {
                operator=(clip);
            } else {
                mOldClip = current;
                Rectangle r{current.x, current.y, current.w, current.h};
                r = r.intersection(clip);
                current = SDL_Rect{r.point.x, r.point.y, r.size.w, r.size.h};
                mStatus = SDL_RenderSetClipRect(mContext.get(), &current);
            }
            return *this;
        }
    };

    /**
     * @brief Rose object error codes.
     */
    enum class RoseErrorCode {
        OK [[maybe_unused]],                         ///< No error
        ROSE_EXCEPTION [[maybe_unused]],             ///< Exception thrown and caught in main().
        SDL_WINDOW_CREATE,          ///< Error while creating the SDL_Window
        SDL_RENDERER_CREATE,        ///< Error while creating the SDL_Renderer
        XDG_PATH_FAIL [[maybe_unused]],              ///< Insufficient information to generate standard XDG Base Directories.
    };

    /**
     * @class GraphicsModel
     */
    class GraphicsModel {
    protected:
        SdlWindow mSdlWindow{};         ///< The SDL_Window which provides the application "Screen"

        Context mContext{};             ///< The graphics context used by the application graphics model.

        bool mRunEventLoop{true};       ///< Event loop continues while this is true.

        bool mRedrawBackground{true};   ///< When true the background Texture needs to be redrawn.

        Texture mBackground{};          ///< The background Texture.

        uint32_t mFrame{};              ///< The rendering frame.

        std::vector<Rectangle> mDisplayBounds{};

    public:
        RoseErrorCode ErrorCode{RoseErrorCode::OK};

        bool initialize();

//        void eventLoop(std::shared_ptr<Screen> &screen);

        void eventLoop();

        /**
         * @brief Draw the screen.
         * @details Screen drawing is accomplished in two steps. If/when the background needs to be redrawn
         * (mRedrawBackground is true) the background widgets (the majority of them) are rendered to the
         * mBackground Texture which is then rendered to the frame buffer.<p/>
         * If mRedrawBackground is false, but mAnimation is true mBackground is rendered to the screen, then
         * animated Widgets, those that change at sub-second rates, are rendered to the screen on top of the
         * background.<p/>
         * If neither mRedrawBackground nor mAnimation are true no rendering of the screen is required.
         * @param screen The Screen object to draw.
         */
//        void drawAll(std::shared_ptr<Screen> &screen);

        std::function<void(SDL_Event)> eventCallback{};

        Context& context() { return mContext; }

        [[nodiscard]] auto currentDisplayIndex() const {
            return static_cast<size_t>(SDL_GetWindowDisplayIndex(mSdlWindow.get()));
        }

        [[maybe_unused]] Rectangle displayBounds() {
            return mDisplayBounds.at(currentDisplayIndex());
        }

        [[maybe_unused]] Rectangle displayBounds(size_t displayIndex) {
            if (displayIndex >= mDisplayBounds.size())
                return mDisplayBounds.at(0);

            return mDisplayBounds.at(displayIndex);
        }

        [[maybe_unused]] Rectangle screenRectangle() {
            Rectangle screenRectangle{};
            SDL_GetWindowSize(mSdlWindow.get(), &screenRectangle.size.w, &screenRectangle.size.h);
            return screenRectangle;
        }

        [[maybe_unused]] void redrawBackground() { mRedrawBackground = true; }

        [[maybe_unused]] [[nodiscard]] Padding windowBorders() const noexcept {
            Padding p{};
            SDL_GetWindowBordersSize(mSdlWindow.get(), &p.topLeft.y, &p.topLeft.x, &p.botRight.y, &p.botRight.x);
            return p;
        }

        [[maybe_unused]] SdlWindow& getSdlWindow() {
            return mSdlWindow;
        }
    };

} // rose

#endif //ROSE2_GRAPHICSMODEL_H
