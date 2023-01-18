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
#include <Widget.h>
#include <exception>
#include <functional>
#include <stack>
#include <queue>
#include <ranges>

namespace rose {
    class Application;
    class Window;

    /**
     * @class Screen
     * @brief The top level widget of a tree managed by a Window.
     * @details A Screen is derived from a Widget but is private to Application and is inserted on
     * top of all the individual trees managed by a Window.
     */
    class Screen : public Widget {
    protected:
        constexpr static std::string_view ClassName = "Screen";

        std::weak_ptr<Window> mWindowPtr{};

    public:
        Screen() = default;

        Screen(const Screen&) = delete;
        Screen(Screen &&) = default;
        Screen& operator=(const Screen &) = delete;
        Screen& operator=(Screen &&) = default;

        Screen(const std::shared_ptr<Window>& windowPtr, const Size& size);

        /**
         * @brief Perform initial Gadget layout.
         * @details The Gadget has an opportunity to specify its own desiredSize by overriding this method.
         * @param drawLocation
         * @param mgrPadding
         */
        bool initialLayout(Context &context) override {
            immediateGadgetLayout();
            return Widget::initialLayout(context);
        }

        ~Screen() override = default;

        /**
         * @brief Events don't propagate beyond a Screen.
         * @return false
         */
        bool enterLeaveEvent(bool, Uint32 ) override { return false; }

        /**
         * @brief Events don't propagate beyond a Screen.
         * @return false
         */
        bool mouseButtonEvent(const SDL_MouseButtonEvent &) override { return false; }

        /**
         * @brief Change the screen size.
         * @param size The new Size.
         */
        void changeSize(const Size& size);

        std::weak_ptr<Application> getApplication();

        std::weak_ptr<Window> getWindow();
    };

    /**
     * @class Window
     */
    class Window : public std::enable_shared_from_this<Window> {
        bool mNeedsLayout{true};            ///< True if window or a contained Gadget needs layout.
        bool mNeedsDrawing{true};           ///< True if window or a contained Gadget needs drawing.

        SdlWindow mSdlWindow{};
        Context mContext{};
        std::vector<Rectangle> mDisplayBounds{};

        std::weak_ptr<Application> mApplicationPtr{};

        std::vector<std::shared_ptr<Screen>> mScreens{};

        std::vector<std::shared_ptr<Gadget>> mFocusChain{};

    public:
        Window() = default;

        explicit Window(std::shared_ptr<Application>& applicationPtr) : mApplicationPtr(applicationPtr) {}

        static std::shared_ptr<Window> createWindow() {
            return std::make_shared<Window>();
        }

        Window(const Window&) = delete;
        Window(Window &&) = default;
        Window& operator=(const Window &) = delete;
        Window& operator=(Window &&) = default;

        ~Window() = default;

        /**
         * @return The current value of the needs layout flag.
         */
        [[maybe_unused]] bool needsLayout() const { return mNeedsLayout; }

        /**
         * @return The current value of the needs drawing flag.
         */
        [[maybe_unused]] bool needsDrawing() const { return mNeedsDrawing; }

        /**
         * @brief Sets the needs layout flag to true and performs the same operation on the Application.
         */
        void setNeedsLayout();

        /**
         * @brief Sets the needs drawing flag to true and performs the same operation on the Application.
         */
        void setNeedsDrawing();

        /**
         * @brief Accessor for the Window graphics context.
         * @return a Context.
         */
        Context& context() { return mContext; }

        /**
         * @brief Accessor for the underlying SdlWindow.
         * @return The SdlWindow.
         */
        [[maybe_unused]] SdlWindow& sdlWindow() { return mSdlWindow; }

        /**
         * @brief Accessor for the current Application Theme values.
         * @return Theme.
         */
        Theme& getTheme();

        /**
         * @brief Accessor for the Application pointer.
         * @return std::weak_ptr<Application>
         */
        std::weak_ptr<Application> getApplication();

        /**
         * @brief Locate a Gadget that passes the provided unary predicate.
         * @param lambda a unary predicate.
         * @return std::shared_ptr<Gadget> which will be empty if no matching Gadget is found.
         */
        std::shared_ptr<Gadget> findGadget(const std::function< bool(std::shared_ptr<Gadget>&) >& lambda);

        /**
         * @brief Get the SDL Window ID of the associated SdlWindow.
         * @return The window ID.
         */
        auto windowID() { return SDL_GetWindowID(mSdlWindow.get()); }

        /**
         * @brief Provide a weak pointer to this.
         * @return std::weak_ptr<Window>
         */
        auto weakPtr() { return std::weak_ptr<Window>(shared_from_this()); }

        /**
         * @brief Indicate to the window management system that this window is resizable, or not
         * @param resizeable True if the window should be resizeable.
         */
        [[maybe_unused]] void setResizeable(bool resizeable) {
            SDL_SetWindowResizable(mSdlWindow.get(), resizeable ? SDL_TRUE : SDL_FALSE);
        }

        /**
         * @brief Set the background color for all active screens.
         * @param background the background color.
         */
        [[maybe_unused]] void setBackgroundColor(const Color &background) {
            mNeedsDrawing = true;
            for (auto &screen : mScreens) {
                screen->setBackground(background);
            }
        }

        /**
         * @brief Layout the contents of the window.
         * @details Layout is a two stage process.
         * Stage one is exploratory. Layout managers gather data on the Gadgets they have to layout.
         * Stage two is executive. Layout managers tell Gadgets where they are positioned and how much space
         * they have to occupy.
         */
        void layout();

        /**
         * @brief Draw the contents of the window.
         * @details The scene is rendered from the bottom up (root of the tree to the leaves) in preorder.
         */
        void draw();

        template<class UiType>
        [[maybe_unused]] std::optional<std::shared_ptr<UiType>> gadget(size_t idx = 0) {
            if (idx < mScreens.size()) {
                if (auto wdg = std::dynamic_pointer_cast<UiType>(mScreens.at(idx)); wdg) {
                    return wdg;
                }
            }
            return std::nullopt;
        }

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
         * @brief Preorder traversal of a Widget tree to apply a lambda to each Gadget.
         * @details The tree is traversed starting from the specified Gadget. Gadgets which are also Widgets are
         * traversed in turn. The provided lambda is applied to all Gadgets. Traversal is preorder.
         * @param topGadget The Gadget to start traversal from.
         * @param lambda The lambda to apply.
         */
        static void gadgetTraversal(std::shared_ptr<Screen> &topGadget,
                                    const std::function< void(std::shared_ptr<Gadget>&) >& lambda);

        /**
         * @brief Post-order traversal of a Widget tree to find a leaf Gadget.
         * @details The tree is traversed starting from the specified Gadget. Gadgets which are also Widgets are
         * traversed in turn. The provided lambda is applied to all Gadgets. Only Gadgets for which the lambda
         * returns true are searched.
         * @param topGadget The Gadget to start traversal from.
         * @param lambda The lambda to apply.
         */
        static std::shared_ptr<Gadget> gadgetFindLast(std::shared_ptr<Screen> &topGadget,
                                     const std::function< bool(std::shared_ptr<Gadget>&) >& lambda);

        /**
         * @brief Clear all focus flags for Gadgets attached to this Window using gadgetTraversal.
         */
        [[maybe_unused]] [[maybe_unused]] void clearAllFocusFlags();

        void initialize(const std::shared_ptr<Application>& applicationPtr, const std::string &title, Size initialSize,
                        const Point &initialPosition, uint32_t extraFlags);

        void changeSize(const Size& size);
    };

} // rose

#endif //ROSE2_WINDOW_H
