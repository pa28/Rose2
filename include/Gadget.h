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

    class Screen;

    class Application;

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

        constexpr static std::string_view ClassName = "Gadget";

        bool mNeedsLayout{true};            ///< True if application or a contained Gadget needs layout.
        bool mNeedsDrawing{true};           ///< True if application or a contained Gadget needs drawing.
        bool mNeedsThemeValues{true};       ///< True if Gadget needs to load Theme values.

        static std::weak_ptr<Application> mApplicationPtr;

        std::vector<DecoratorFunction> mDecorators{};

        constexpr static GadgetType ThisType = GadgetType::Gadget;

        [[maybe_unused]] std::string_view mName{};

        std::weak_ptr<Gadget> manager{};  ///< Pointer to the current manager of this Gadget.

        struct VisualMetrics {
            /**
             * @brief The drawing location provided by the manager.
             * @details This is applied as a Point offset to the render rectangles as the Gadget is drawn.
             */
            Point drawLocation{};

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

            /**
             * True if the Gadget has focus.
             */
            [[maybe_unused]] bool mHasFocus{};
        } mVisualMetrics;

    public:
        Gadget() = default;

        Gadget(const Gadget &) = delete;

        Gadget(Gadget &&) = default;

        Gadget &operator=(const Gadget &) = delete;

        Gadget &operator=(Gadget &&) = default;

        virtual ~Gadget() = default;

        virtual const std::string_view& className() const { return Gadget::ClassName; }

        [[nodiscard]] [[maybe_unused]] const std::string_view& name() { return mName; }

        [[nodiscard]] bool isManaged() const {
            using wt = std::weak_ptr<Widget>;
            bool a = manager.owner_before(wt{});
            bool b = wt{}.owner_before(manager);
            return !(!a && !b);
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
         * @details Internal alignment padding is used to align Gadgets inside of managers. Internal alignment
         * may be set directly by using the getVisualMetrics() accessor function; however this method should
         * be used as it allows managers to place the padding in an appropriate location.
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
         * @details Called by managers (derived from Singlet or Widget) when they are called to be constrained.
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

        [[maybe_unused]] [[nodiscard]] virtual GadgetType gadgetType() const { return Gadget::ThisType; }

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
         */
        virtual void draw(Context &context, Point drawLocation);

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
         * @return std::shared_ptr<Screen> pointing to the screen, or empty.
         */
        std::shared_ptr<Screen> getScreen();

        /**
         * @brief Get the Theme object from the Application.
         * @return A Theme&
         */
        std::shared_ptr<Theme> getTheme();

        /**
         * @brief Load values from the theme.
         * @return A shared_ptr to the current Theme if Theme values need to be loaded, nullptr otherwise.
         */
        [[maybe_unused]] virtual std::shared_ptr<Theme> getThemeValues();

        void getApplicationPtr();

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

    class Singlet;
    /**
     * @class Builder
     * @brief Base class for object builders for specializations of Gadgets. Once complete
     * the Builder can be passed to Widget::manage().
     */
    class Builder {
        friend class Widget;
        friend class Singlet;

    protected:
        std::shared_ptr<Gadget> gadget;     ///< Where the Gadget is built.
    public:
        Builder() = delete;

        virtual ~Builder() = default;

        explicit Builder(std::shared_ptr<Gadget> g) : gadget(std::move(g)) {}

        explicit operator bool() { return true; }

        /**
         * @brief Manage the built Gadget by the specified Widget.
         * @param widget the manager.
         */
        void operator>>(Widget &widget);

        /**
         * @brief Manage the built Gadget by the specified Singlet.
         * @param singlet the manager.
         */
        void operator>>(Singlet &singlet);

        /**
         * @brief Manage the built Gadget by the specified Widget.
         * @param widget a std::shared_ptr to the manager.
         */
        void operator>>(const std::shared_ptr<Widget> &widget);

        /**
         * @brief Manage the built Gadget by the specified Singlet.
         * @param singlet a std::shared_ptr to the manager.
         */
        void operator>>(const std::shared_ptr<Singlet> &singlet);

        /**
         * @brief Manage the contents of this Builder by the contents of another Builder (presumably a WidgetBuilder.
         * @details Probably more complicated than its worth.
         * @param builder The WidgetBuilder.
         */
        void operator>>(Builder &builder);

        /**
         * @brief Get a std::shared_ptr to a Widget from a builder, if it contains a Widget.
         * @tparam T the type of gadget to extract.
         * @return std::shared_ptr<T> which may be empty.
         */
        template<class T>
        auto get() {
            return std::dynamic_pointer_cast<T>(gadget);
        }
    };

    template<class B>
    concept IsBuilder = std::derived_from<B, Builder>;

    class GadgetBuilder : public Builder {
    public:
        explicit GadgetBuilder(std::shared_ptr<Gadget> g) : Builder(std::move(g)) {}

        GadgetBuilder() : Builder(std::make_shared<Gadget>()) {}

        ~GadgetBuilder() override = default;

        /**
         * @brief Set the Gadget name.
         * @tparam T StringLike concept.
         * @param t String like value.
         * @return this builder
         */
        template<typename T>
        requires StringLike<T>
        auto name(T t) {
            gadget->mName = t;
            return *this;
        }

        /**
         * @brief Set the desired layout rectangle.
         * @param rectangle containing the desired layout
         * @return this builder
         */
        [[maybe_unused]] auto layout(const Rectangle &rectangle) {
            gadget->mVisualMetrics.desiredSize = rectangle.size;
            gadget->mVisualMetrics.drawLocation = rectangle.point;
            return *this;
        }

        /**
         * @brief Set the desired layout rectangle from a Point and Size.
         * @param point The Point.
         * @param size The Size.
         * @return this builder.
         */
        [[maybe_unused]] GadgetBuilder &layout(const Point &point, const Size &size) {
            gadget->mVisualMetrics.desiredSize = size;
            gadget->mVisualMetrics.drawLocation = point;
            return *this;
        }

        /**
         * @brief Set the desired layout rectangle from individual values.
         * @tparam Tx Type of X
         * @tparam Ty Type of Y
         * @tparam Tw Type of W
         * @tparam Th Type of H
         * @param X The X coordinate
         * @param Y The Y coordinate
         * @param W The Width
         * @param H The Height
         * @return this builder.
         */
        template<typename Tx, typename Ty, typename Tw, typename Th>
        [[maybe_unused]] auto layout(Tx X, Ty Y, Tw W, Th H) {
            static_assert(std::is_convertible_v<Tx, ScreenCoordType> && std::is_convertible_v<Ty, ScreenCoordType> &&
                          std::is_convertible_v<Tw, ScreenCoordType> && std::is_convertible_v<Th, ScreenCoordType>,
                          "Arguments to Size() must be convertable to ScreenCoordType");
            gadget->mVisualMetrics.desiredSize = Size(W, H);
            gadget->mVisualMetrics.drawLocation = Point(X, Y);
            return *this;
        }

        /**
         * @brief Set the background backgroundColor of the Gadget
         * @param backgroundColor
         * @return this Builder
         */
        [[maybe_unused]] auto background(const Color &backgroundColor) {
            gadget->mVisualMetrics.background = backgroundColor;
            return *this;
        }

        /**
         * @brief Set DecoratorFunction on the Gadget
         * @param decoratorFunction
         * @return this Builder
         */
        [[maybe_unused]] auto decorator(DecoratorFunction&& decoratorFunction) {
            gadget->setDecorator(std::move(decoratorFunction));
            return *this;
        }
    };

} // rose

/**
 * @brief Manage the built Gadget by the specified Widget
 * @tparam B Concept rose::IsBuilder<B> requires B is derived from Builder.
 * @param widget The managing Widget
 * @param builder The builder of the Gadget
 */
template<typename B>
requires rose::IsBuilder<B>
inline void operator << (std::shared_ptr<rose::Widget> &widget, B builder) {
    builder >> widget;
}

#endif //ROSE2_GADGET_H
