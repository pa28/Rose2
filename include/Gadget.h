//
// Created by richard on 31/12/22.
//

/*
 * Gadget.h Created by Richard Buckley (C) 31/12/22
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

//#define DEBUG_GADGET_DRAW

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

#ifdef DEBUG_GADGET_DRAW
    static Color ClipRectangleDebugColor{1.0, 0.0, 0.0, 1.0 };
    static Color BorderRectangleDebugColor{ 0.0, 0.0, 1.0, 1.0 };
    static Color RenderRectangleDebugColor{0.0, 1.0, 0.0, 1.0};
#endif

    /**
     * @class Gadget
     * @brief The simplest UI element. A Gadget renders a graphical representation of its state. It does not
     * manage any other scene tree elements.
     */
    class Gadget : public std::enable_shared_from_this<Gadget> {
    protected:
        friend class GadgetBuilder;
        friend class Window;

        using DecoratorFunction = std::function<void(Context &, Gadget &)>;

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
            bool mHasFocus{};
        } mVisualMetrics;

    public:
        Gadget() = default;

        Gadget(const Gadget &) = delete;

        Gadget(Gadget &&) = default;

        Gadget &operator=(const Gadget &) = delete;

        Gadget &operator=(Gadget &&) = default;

        virtual ~Gadget() = default;

        [[nodiscard]] bool isManaged() const {
            using wt = std::weak_ptr<Widget>;
            bool a = manager.owner_before(wt{});
            bool b = wt{}.owner_before(manager);
            return !(!a && !b);
        }

        /**
         * @brief Determine if a point is 'inside' a Gadget.
         * @param point The point to test.
         * @return true if point is 'inside' Gadget.
         */
        [[nodiscard]] bool containsPoint(const Point &point) const {
            auto r = (mVisualMetrics.clipRectangle + mVisualMetrics.drawLocation).contains(point);
            return r;
        }

        /**
         * @brief Force initial gadget layout avoiding virtual method.
         * @details This may be required for Widgets to call after they have configured their Gadgets.
         * @return
         */
        bool forceInitialGadgetLayout();

        /**
         * @brief Generate content, determine size and set the initial values of layout rectangles.
         * @param context Graphics context
         * @return true if successful.
         */
        virtual bool initialLayout(Context &) { return forceInitialGadgetLayout(); }

        /**
         * @brief Adjust size, if necessary to fit into the constrained size. Finalize layout and visuals.
         * @details Called by managers (derived from Singlet or Widget) when they are called to be constrained.
         * @param context Graphics context
         * @param constraint If set, the Gadget must render itself within the constrained size.
         * @return true if successful.
         */
        virtual bool finalLayout(Context &, const Size &) { return false; }

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
        [[nodiscard]] auto getManager() const { return manager; }

        /**
         * @brief Draw this Gadget.
         * @param context The graphics context to use.
         */
        virtual void draw(Context &context, Point drawLocation);

        [[maybe_unused]] void setSize(const Size &size) { mVisualMetrics.desiredSize = size; }

        [[maybe_unused]] void setDrawLocation(const Point &point) {
            mVisualMetrics.drawLocation = point;
        }

        void setBackground(const Color &color) { mVisualMetrics.background = color; }

        /**
         * @brief Set the Gadget name.
         * @tparam String StringLike concept.
         * @param string String like value.
         * @return this builder
         */
        template<typename String>
        requires StringLike<String>
        void setName(String string) { mName = string; }

        [[maybe_unused]] [[nodiscard]] auto getName() const { return mName; }

        /**
         * @brief Get the Screen which ultimately hosts this Gadget.
         * @return std::shared_ptr<Screen> pointing to the screen, or empty.
         */
        std::shared_ptr<Screen> getScreen();

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
    };

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

        void reset() { gadget.reset(); }

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

        [[maybe_unused]] auto background(const Color &color) {
            gadget->mVisualMetrics.background = color;
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
