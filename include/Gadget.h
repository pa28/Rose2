//
// Created by richard on 31/12/22.
//

/*
 * Gadget.h Created by Richard Buckley (C) 31/12/22
 */

/**
 * @file Gadget.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 31/12/22
 * @brief Header file for the Gadget, Builder and GadgetBuilder classes.
 * @details A Gadget is the simplest visual UI element managing an area of an application window display. A Gadget
 * does not manage any other Gadgets.<br/>
 * Builder is an abstract factory class for Gadgets. Each Gadget class has (or should have) a corresponding Builder
 * class. The Builder is used to properly create a Gadget, set parameters appropriate for the application, and
 * insert the Gadget in the scene tree of the application.<br/>
 * GadgetBuilder is the Builder class for Gadgets.
 */

#ifndef ROSE2_GADGET_H
#define ROSE2_GADGET_H

#include <string_view>
#include <memory>
#include <array>
#include <vector>
#include <utility>
#include <algorithm>
#include <concepts>
#include <exception>
#include <Rose.h>
#include <Color.h>
#include <GraphicsModel.h>
#include <Animation.h>
#include <fmt/format.h>

namespace rose {

    /**
     * @class SceneTreeError
     * @brief Thrown when there is an un-recoverable error with the scene tree or the manipulation of it.
     */
    class SceneTreeError : public std::runtime_error {
    public:
        explicit SceneTreeError(const std::string &whatArg) : std::runtime_error(whatArg) {}
    };


    /**
     * @class Gadget
     * @brief A Gadget is a visual UI element that does not manage any other elements.
     */
    class Widget;

    class LayoutManager;

    class Window;

    class Screen;

    class Application;

    class Animation;

    /**
     * @brief A DecoratorFunction can be attached to a Gadget to be called when Gadget::Draw is called.
     */
    class Gadget;
    using DecoratorFunction = std::function<void(Context &, Gadget &)>;

    /**
     * @class Gadget
     * @brief The simplest UI element. A Gadget renders a graphical representation of its state. It does not
     * manage any other scene tree elements.
     */
    class Gadget : public std::enable_shared_from_this<Gadget> {
    protected:
        friend class GadgetBuilder;
        friend class Window;

        constexpr static std::string_view ClassName = "Gadget";     ///< class name

        bool mNeedsLayout{true};            ///< True if application or a contained Gadget needs layout.
        bool mNeedsDrawing{true};           ///< True if application or a contained Gadget needs drawing.
        bool mIsInitialized{false};         ///< True if the Gadget has been initialized on connection to the tree.

        static std::weak_ptr<Application> mApplicationPtr;      ///< Access to the Application object.

        std::vector<DecoratorFunction> mDecorators{};           ///< List of decorator functions.

        [[maybe_unused]] std::string mName{};                   ///< Gadget instance name.

        std::weak_ptr<Gadget> manager{};  ///< Pointer to the current manager of this Gadget.

        struct VisualMetrics {
            /**
             * @brief The drawing location provided by the manager.
             * @details This is applied as a Point offset to the render rectangles as the Gadget is drawn.
             */
            Point drawLocation{};

            /**
             * @brief The last computed drawing location.
             */
            Point lastDrawLocation{};

            /**
             * @brief The content drawing size requested by the Gadget.
             */
            Size desiredSize{};

            /**
             * @brief Padding used by the layout manager to position the Gadget.
             * @details This padding is placed inside the border changing the size inside the border to
             * align the contents with other gadgets. @see outerAlignmentPadding.
             */
            Padding innerAlignmentPadding{};

            /**
             * @brief Padding used by the Gadget.
             * @details This can be used for better visual presentation, and/or managing the location
             * of the Gadget graphics within the allocated space. Included in the clip rectangle.
             */
            Padding gadgetPadding{};

            /**
             * @brief The Gadget content rendering rectangle.
             * @details May be used to render Textures directly, or used to derive rectangles for rendering
             * peaces. Offset by drawLocation before use.
             */
            Rectangle renderRect{};

            /**
             * @brief The clipping rectangle for the Gadget.
             * @details The Gadget must not draw outside this rectangle. This is the location and size
             * that all other UI objects will expect the Gadget to manage. Gadget padding and
             * graphics are all rendered inside this rectangle. Offset by drawLocation before use.
             */
            Rectangle clipRectangle{};

            /**
             * @brief The background color of the Gadget.
             * @details If set this color is rendered to the clipRectangle before any other drawing.
             */
            Color background{};
            Color animateBackground{};

            /**
             * True if the Gadget has focus.
             */
            [[maybe_unused]] bool mHasFocus{};
        } mVisualMetrics;

        [[maybe_unused]] virtual std::shared_ptr<Theme> getThemeValues();

    public:
        std::unique_ptr<Animation> mAnimationPtr{};

        Gadget() = default;
        explicit Gadget(const std::shared_ptr<Theme>& ) : Gadget() {}
        Gadget(const Gadget &) = delete;
        Gadget(Gadget &&) = default;
        Gadget &operator=(const Gadget &) = delete;
        Gadget &operator=(Gadget &&) = default;
        virtual ~Gadget() = default;

        virtual void completeCompositeConstruction(std::shared_ptr<Theme>& ) {}

        /**
         * @brief Get initialization state of the Gadget.
         * @return true if initialized.
         */
        bool isInitialized() const { return mIsInitialized; }

        /**
         * @brief Get the Gadget class name.
         * @return std::string_view&
         */
        [[maybe_unused]] virtual const std::string_view& className() const { return Gadget::ClassName; }

        /**
         * @brief Get the Gadget instance name.
         * @return
         */
        [[nodiscard]] [[maybe_unused]] const std::string& name() { return mName; }

        /**
         * @brief Determine if the Gadget is managed.
         * @return true if managed.
         */
        [[nodiscard]] bool isManaged() const {
            using wt = std::weak_ptr<Widget>;
            bool a = manager.owner_before(wt{});
            bool b = wt{}.owner_before(manager);
            return !(!a && !b);
        }

        /**
         * @brief Initialize the gadget.
         * @details Called when the initial scene tree is attached to the application. This is the chance for a
         * Gadget to finalize construction that requires data from the Application or other parts of the scene
         * tree.
         */
        virtual void initialize() {
            mIsInitialized = true;
            if (mAnimationPtr)
                mAnimationPtr->initialize();
        }

        /**
         * @return The current value of the needs layout flag.
         */
        [[maybe_unused]] bool needsLayout() const { return mNeedsLayout; }

        /**
         * @return The current value of the needs drawing flag.
         */
        [[maybe_unused]] bool needsDrawing() const { return mNeedsDrawing; }

        /**
         * @brief Sets the needs layout flag to true.
         */
        void setNeedsLayout();

        /**
         * @brief Sets the needs drawing flag to true.
         */
        void setNeedsDrawing();

        /**
         * @brief Determine if a point is 'inside' a Gadget.
         * @param point The point to test.
         * @return true if point is 'inside' Gadget.
         */
        [[nodiscard]] bool containsPoint(const Point &point) const {
            auto r = (mVisualMetrics.clipRectangle + mVisualMetrics.lastDrawLocation).contains(point);
            return r;
        }

        /**
         * @brief Immediate Gadget layout calculation.
         * @details Compute the layout of the Gadget locally using only what is know to the Gadget.
         * @return
         */
        virtual bool immediateGadgetLayout();

        /**
         * @brief Set internal alignment padding.
         * @details Internal alignment padding is used to align Gadgets inside of manager. Internal alignment
         * may be set directly by using the getVisualMetrics() accessor function; however this method should
         * be used as it allows manager to place the padding in an appropriate location.
         * @param padding the padding.
         */
        [[maybe_unused]] virtual void setInternalAlignmentPadding(const Padding& padding) {
            mVisualMetrics.innerAlignmentPadding = padding;
        }

        /**
         * @brief Generate content, determine size and set the initial values of layout rectangles.
         * @param context Graphics context
         * @return true if successful.
         */
        virtual bool initialLayout(Context &) { mNeedsLayout = false; return immediateGadgetLayout(); }

        /**
         * @brief Adjust size, if necessary to fit into the constrained size. Finalize layout and visuals.
         * @details Called by manager (derived from Singlet or Widget) when they are called to be constrained.
         * @param context Graphics context
         * @param constraint If set, the Gadget must render itself within the constrained size.
         * @return true if successful.
         */
        [[maybe_unused]] virtual bool finalLayout(Context &, const Size &) { return false; }

        /**
         * @brief Find a constrained layout for this Gadget.
         * @details This method is only called when there is insufficient room to render the Gadget.
         */
        virtual void constrainedGadgetLayout(Context &, Size) {}

        /**
         * @brief Get access to the visual metrics of the Gadget so they may be manipulated directly.
         * @details This should only be done by methods involved in layout management.
         * @return A reference to the VisualMetrics structure.
         */
        VisualMetrics &getVisualMetrics() { return mVisualMetrics; }

        /**
         * @brief Set the pointer to the current manager of this Gadget.
         * @param gadget
         */
        void managedBy(const std::shared_ptr<Gadget> &gadget);

        /**
         * @brief Get the current manager of this Gadget if any.
         * @return A pointer (possibly empty) to the Gadget's manager.
         */
        [[maybe_unused]] [[nodiscard]] auto getManager() const { return manager; }

        /**
         * @brief Draw this Gadget.
         * @param context The graphics context to use.
         * @param drawLocation The point at which to draw the Gadget.
         */
        virtual void draw(Context &context, Point drawLocation);

        /**
         * @brief Expose a portion of the Gadget.
         * @details This method is called when only a portion of the screen has to be redrawn because it is
         * exposed by removal of an overlying Gadget or a Gadget has changed due to animation or other
         * modifying process.
         * @param context The graphics context to use.
         * @param exposed The exposed rectangle.
         */
        virtual void expose(Context &context, Rectangle exposed);

        /**
         * @brief Compute the intersection of the exposed rectangle with the Gadget.
         * @details If there is no intersection an unset Rectangle is returned.
         * @param exposed The exposed area
         * @return The intersection, if any.
         */
        Rectangle exposure(Rectangle exposed) const {
            return (mVisualMetrics.clipRectangle + mVisualMetrics.lastDrawLocation).intersection(exposed);
        }

        /**
         * @brief Set the desired size of the Gadget
         * @param size the Size.
         */
        [[maybe_unused]] void setSize(const Size &size) { mVisualMetrics.desiredSize = size; }

        /**
         * @brief Save the draw location for later use.
         * @param point the draw location.
         */
        [[maybe_unused]] void setDrawLocation(const Point &point) {
            mVisualMetrics.drawLocation = point;
        }

        /**
         * @brief Set the Gadget background backgroundColor.
         * @param backgroundColor
         */
        void setBackground(const Color &backgroundColor) { mVisualMetrics.background = backgroundColor; }

        /**
         * @brief Set the Gadget name.
         * @tparam String StringLike concept.
         * @param string String like value.
         * @return this builder
         */
        template<typename String>
        requires StringLike<String>
        [[maybe_unused]] void setName(String string) { mName = string; }

        [[maybe_unused]] [[nodiscard]] auto getName() const { return mName; }

        /**
         * @brief Get the Screen which ultimately hosts this Gadget.
         * @return std::shared_ptr<Screen> pointing to the Screen, or empty.
         */
        std::shared_ptr<Screen> getScreen();

        /**
         * @brief Get the Window which ultimately hold this Gadget.
         * @return std::shard_ptr<Window> point to the Window, or empty.
         */
        std::shared_ptr<Window> getWindow();

        /**
         * @brief Calculate and return the current exposure rectangle for a Gadget.
         * @details The exposure rectangle is the clipping rectangle translated to the last drawing location.
         * @return The exposure Rectangle.
         */
        Rectangle getExposedRectangle() const {
            if (mVisualMetrics.lastDrawLocation && mVisualMetrics.clipRectangle)
                return mVisualMetrics.clipRectangle + mVisualMetrics.lastDrawLocation;
            return Rectangle{};
        }

        /**
         * @brief Get the Theme object from the Application.
         * @return A Theme&
         */
        std::shared_ptr<Theme> getTheme();

        /**
         * @brief Get and store a pointer to the Application object.
         * @details The pointer is stored in mApplicationPtr.
         */
        std::shared_ptr<Application> getApplicationPtr();

        /**
         * @brief Receive Enter/Leave events.
         * @details No action is implemented in the Gadget base class. If the derived Gadget/Widget does not define
         * an override method the event will be passed up the Gadget tree until is is accepted or a non-managed
         * Widget (the top level for a Screen) is encountered.
         * @param enter true if the mouse entered the Gadget.
         * @param timestamp the time of the event.
         * @return true if the event was processed.
         */
        virtual bool enterLeaveEvent(bool enter, Uint32 timestamp);

        /**
         * @brief Receive mouse button events.
         * @details No action is implemented in the Gadget base class. If the derived Gadget/Widget does not define
         * an override method the event will be passed up the Gadget tree until is is accepted or a non-managed
         * Widget (the top level for a Screen) is encountered.
         * @param e the SDL_MouseButtonEvent.
         * @return true if the event was processed.
         */
        virtual bool mouseButtonEvent(const SDL_MouseButtonEvent &e);

        /**
         * @brief Add a decorator function to the list of decorators
         * @param decoratorFunction the decorator function.
         */
        void setDecorator(DecoratorFunction&& decoratorFunction) {
            mDecorators.emplace_back(decoratorFunction);
        }
    };

    /**
     * @brief A DecoratorFunction to draw the Gadget background in the Theme background color.
     * @param context The graphics context to draw with.
     * @param gadget The Gadget to background.
     */
    [[maybe_unused]] [[maybe_unused]] void ThemeBackgroundDecorator(Context& context, Gadget& gadget);


} // rose


#endif //ROSE2_GADGET_H
