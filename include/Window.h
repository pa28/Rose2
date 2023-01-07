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
    class Application;
    class Window;

    /**
     * @class Screen
     * @brief The top level widget of a tree managed by a Window.
     * @details A Screen is derived from a Widget but is private to Application and is inserted on
     * top of all the individual trees managed by a Window.
     */
    class Screen : public Widget {
    private:
        std::weak_ptr<Window> mWindowPtr{};

    public:
        Screen() = default;

        Screen(const Screen&) = delete;
        Screen(Screen &&) = default;
        Screen& operator=(const Screen &) = delete;
        Screen& operator=(Screen &&) = default;

        Screen(const std::shared_ptr<Window>& windowPtr, const Size& size) {
            mWindowPtr = windowPtr;
            mName = "Top";
            mVisualMetrics.desiredSize = size;
            mVisualMetrics.background = color::TransparentBlack;
        }

        /**
         * @brief Perform initial Gadget layout.
         * @details The Gadget has an opportunity to specify its own desiredSize by overriding this method.
         * @param drawLocation
         * @param mgrPadding
         */
        bool initialGadgetLayout(Context &context) override {
            forceInitialGadgetLayout();
            return Widget::initialGadgetLayout(context);
        }

        /**
         * @brief Draw this Widget and all managed Gadgets.
         * @param context The graphics context to use.
         */
//        void draw(Context &context, Point drawLocation) override {
//            Gadget::draw(context, drawLocation);
//
//            for (const auto& gadget : mGadgetList) {
//                Point gadgetDrawLocation = drawLocation + gadget->getVisualMetrics().drawLocation;
//                gadget->draw(context, gadgetDrawLocation);
//            }
//        }

        ~Screen() override = default;
    };

    /**
     * @class Window
     */
    class Window : public std::enable_shared_from_this<Window> {
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

        Context& context() { return mContext; }

        SdlWindow& sdlWindow() { return mSdlWindow; }

        std::shared_ptr<Gadget> findGadget(const std::function< bool(std::shared_ptr<Gadget>&) >& lambda);

        auto windowID() { return SDL_GetWindowID(mSdlWindow.get()); }

        auto weakPtr() { return std::weak_ptr<Window>(shared_from_this()); }

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
    };

} // rose

#endif //ROSE2_WINDOW_H
