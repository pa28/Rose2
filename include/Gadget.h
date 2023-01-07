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
#include <Rose.h>
#include <Color.h>
#include <GraphicsModel.h>
#include <fmt/format.h>

//#define DEBUG_GADGET_DRAW

namespace rose {

/**
 * @class Gadget
 * @brief A Gadget is a visual UI element that does not manage any other elements.
 */
    class Widget;
    class LayoutManager;

#ifdef DEBUG_GADGET_DRAW
    static Color ClipRectangleDebugColor{1.0, 0.0, 0.0, 1.0 };
    static Color BorderRectangleDebugColor{ 0.0, 0.0, 1.0, 1.0 };
    static Color RenderRectangleDebugColor{0.0, 1.0, 0.0, 1.0};
#endif

    class Gadget {
    public:
        friend class GadgetBuilder;
        friend class Window;
        friend class LayoutManager;

        constexpr static GadgetType ThisType = GadgetType::Gadget;
        [[maybe_unused]] std::string_view mName{};
        std::weak_ptr<Widget> manager{};  ///< Pointer to the current manager of this Gadget.

        struct VisualMetrics {
            /**
             * @brief Set to true when the Gadget layout is constrained.
             * @details Gadgets are constrained when there is insufficient room to render them fully. The application
             * designer may wish to use a more flexible solutions such as a ScrollWidget. The Gadget may modify its
             * rendering to provide a visual cue of the constraint.
             */
            bool constrained{false};

            /**
             * @brief The drawing location provided by the manager.
             * @details This is applied as a Point offset to the render rectangles as the Gadget is drawn.
             */
            Point drawLocation{};

            /**
             * @brief The las position provided to Gadget::Draw.
             * @details This position, along with the remainder of the VisualMetrics, is used to provide aspects
             * of functionality. If a Gadget does not call the base class draw function it should set this value
             * correctly itself to maintain this functionality.
             */
            Point lastDrawLocation{};

            /**
             * @brief The content drawing size requested by the Gadget.
             */
            Size desiredSize{};

            /**
             * @brief The size of the boarder the Gadget is rendering.
             * @details This is included in the clip rectangle.
             */
            int gadgetBorder{};

            /**
             * @brief Padding used by the layout manager to position the Gadget.
             * @details This is not included in the Gadget clip rectangle. The gadget retains its original
             * size and is just pushed around. @see innerAlignmentPadding.
             */
            Padding outerAlignmentPadding{};

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
             * peaces.
             */
            Rectangle renderRect{};

            /**
             * @brief The Gadget border rendering rectangle.
             * @details This rectangle is used to render a border around the Gadget visual.
             */
            Rectangle borderRect{};

            /**
             * @brief The clipping rectangle for the Gadget.
             * @details The Gadget must not draw outside this rectangle. This is the location and size
             * that all other UI objects will expect the Gadget to manage. Gadget borders, padding and
             * graphics are all rendered inside this rectangle.
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
        Gadget(const Gadget&) = delete;
        Gadget(Gadget&&) = default;
        Gadget& operator = (const Gadget &) = delete;
        Gadget& operator = (Gadget &&) = default;

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
            auto r = (mVisualMetrics.borderRect + mVisualMetrics.lastDrawLocation).contains(point);
            return r;
        }

        /**
         * @brief Perform initial Gadget layout.
         * @details The Gadget has an opportunity to specify its own desiredSize by overriding this method.
         * @param drawLocation
         * @param mgrPadding
         */
        virtual bool initialGadgetLayout(Context &) {
            return forceInitialGadgetLayout();
        }

        /**
         * @brief Force initial gadget layout avoiding virtual method.
         * @details This may be required for Widgets to call after they have configured their Gadgets.
         * @param mgrPadding
         * @return
         */
        bool forceInitialGadgetLayout();

        /**
         * @brief Find a constrained layout for this Gadget.
         * @details This method is only called when there is insufficient room to render the Gadget.
         */
        virtual void constrainedGadgetLayout(Context &, Rectangle ) {}

        /**
         * @brief Get access to the visual metrics of the Gadget so they may be manipulated directly.
         * @details This should only be done by methods involved in layout management.
         * @return A reference to the VisualMetrics structure.
         */
        VisualMetrics& getVisualMetrics() { return mVisualMetrics; }

        [[maybe_unused]] [[nodiscard]] virtual GadgetType gadgetType() const { return Gadget::ThisType; }

        /**
         * @brief Set the pointer to the current manager of this Gadget.
         * @param widget
         */
        void managedBy(const std::shared_ptr<Widget>& widget);

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

        virtual ~Gadget() = default;

        [[maybe_unused]] void setSize(const Size &size) { mVisualMetrics.desiredSize = size; }

        [[maybe_unused]] void setDrawLocation(const Point &point) {
            mVisualMetrics.drawLocation = point;
        }

        void setBackground(const Color& color) { mVisualMetrics.background = color; }

        /**
         * @brief Set the Gadget name.
         * @tparam String StringLike concept.
         * @param string String like value.
         * @return this builder
         */
        template<typename String>
        requires StringLike<String>
        auto setName(String string) { mName = string; }

        /**
         * @brief Receive Enter/Leave events.
         * @details If the derived Gadget/Widget does not define an override method the event will be passed
         * up the Gadget tree until is is accepted or a non-managed Widget (the top level for a Screen) is
         * encountered.
         * @param enter true if the mouse entered the Gadget.
         * @param timestamp the time of the event.
         * @return true if the event was processed.
         */
        virtual bool enterLeaveEvent(bool enter, Uint32 timestamp);

    };

    /**
     * @class Builder
     * @brief Base class for object builders for specializations of Gadgets. Once complete
     * the Builder can be passed to Widget::manage().
     */
    class Builder {
        friend class Widget;
    protected:
        std::shared_ptr<Gadget> gadget;     ///< Where the Gadget is built.
    public:
        Builder() = delete;

        virtual ~Builder() = default;

        explicit Builder(std::shared_ptr<Gadget> g) : gadget(std::move(g)) {}

        explicit operator bool () { return true; }

        void reset() { gadget.reset(); }

        /**
         * @brief Manage the built Gadget by the specified Widget.
         * @param widget the manager.
         */
        void operator >> (Widget &widget);

        /**
         * @brief Manage the built Gadget by the specified Widget.
         * @param widget a std::shared_ptr to the manager.
         */
        void operator >> (const std::shared_ptr<Widget>& widget);

        /**
         * @brief Manage the contents of this Builder by the contents of another Builder (presumably a WidgetBuilder.
         * @details Probably more complicated than its worth.
         * @param builder The WidgetBuilder.
         */
        void operator >> (Builder& builder);

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
    concept IsBuilder = std::derived_from<B,Builder>;

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
        [[maybe_unused]] GadgetBuilder& layout(const Point &point, const Size &size) {
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
            gadget->mVisualMetrics.drawLocation = Point(X,Y);
            return *this;
        }

        [[maybe_unused]] auto background(const Color& color) {
            gadget->mVisualMetrics.background = color;
            return *this;
        }
    };

    /**
     * @class Widget
     * @brief A Widget is a visual UI element that does manage other Gadgets.
     * @details The list of managed Gadgets is maintained in a std::vector. Many
     * std::vector methods are exposed allowing colling programs to utilize
     * these methods and range based algorithms.
     */
    class Widget: public std::enable_shared_from_this<Widget>, public Gadget {
        friend class LayoutManager;
        friend class Window;

    protected:
        constexpr static GadgetType ThisType = GadgetType::Widget;

        std::vector<std::shared_ptr<Gadget>> mGadgetList{};  ///< The list of Gadgets managed.

        std::unique_ptr<LayoutManager> mLayoutManager{};

    public:
        Widget() = default;
        Widget(const Widget&) = delete;
        Widget(Widget&&) = default;
        Widget& operator = (const Widget &) = delete;
        Widget& operator = (Widget &&) = default;

        [[nodiscard]] GadgetType gadgetType() const override { return Widget::ThisType; }

        bool initialGadgetLayout(Context &context) override;

        template<class Layout>
        requires std::derived_from<LayoutManager,Layout>
        void setLayoutManager(std::unique_ptr<Layout>&& layout) {
            mLayoutManager = std::move(layout);
        }

        /**
         * @brief Add a Gadget to the management list of this Widget.
         * @details This will include setting this Widget as the manager of the Gadget, which will remove the
         * gadget from the management list of its current manager, if any.
         * @param gadget The Gadget to manage.
         */
        [[maybe_unused]] void manage(std::shared_ptr<Gadget> gadget);

        /**
         * @brief Add a Gadget from a builder to the management list of this Widget
         * @param builder The Builder holding the new Gadget.
         */
        [[maybe_unused]] void manage(Builder &builder);

        /**
         * @brief Get a pointer to the first Gadget on the list of managed Gadgets.
         * @details This does not un-manage the Gadget.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto front() { return mGadgetList.front(); }

        /**
         * @brief Get a pointer to the last Gadget on the list of managed Gadgets.
         * @details This does not un-manage the Gadget.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto back() { return mGadgetList.back(); }

        /**
         * @brief Get a pointer to the Gadget at idx on the list of managed Gadgets.
         * @details This does not un-manage the Gadget. Access is by calling vector::at(idx).
         * @param idx The index of the Gadget to return.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto at(size_t idx) { return mGadgetList.at(idx); }

        /**
         * @brief Get a pointer to the Gadget at idx on the list of managed Gadgets.
         * @details This does not un-manage the Gadget. Access is by calling vector::operator[](idx).
         * @param idx The index of the Gadget to return.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto operator[](size_t idx) { return mGadgetList.operator[](idx); }

        /**
         * @brief Get the count of Gadgets managed by this Widget by calling vector::size().
         * @return The number of Gadgets.
         */
        [[maybe_unused]] auto gadgetCount() const { return mGadgetList.size(); }

        /**
         * @brief Un-manage the Gadget if it is found by std::remove.
         * @param gadget The gadget to remove.
         */
        [[maybe_unused]] void unManage(const std::shared_ptr<Gadget>& gadget);

        /**
         * @brief Get the begin iterator of the Gadget list.
         * @return The return value of vector::begin().
         */
        auto begin() { return mGadgetList.begin(); }

        /**
         * @brief Get the begin iterator of the Gadget list.
         * @return The return value of vector::end().
         */
        auto end() { return mGadgetList.end(); }

        /**
         * @brief Draw this Widget and all managed Gadgets.
         * @param context The graphics context to use.
         */
        void draw(Context &context, Point drawLocation) override;

        ~Widget() override = default;
    };

    class WidgetBuilder : public GadgetBuilder {
    public:
        [[maybe_unused]] explicit WidgetBuilder(std::shared_ptr<Widget> w) : GadgetBuilder(std::move(w)) {}

        WidgetBuilder() : GadgetBuilder(std::make_shared<Widget>()) {}

        ~WidgetBuilder() override = default;
    };

    class LayoutManager {
    protected:
        static auto getGadgetList(std::shared_ptr<Widget> &widget) {
            return widget->mGadgetList;
        }

    public:
        LayoutManager() = default;
        LayoutManager(const LayoutManager&) = delete;
        LayoutManager(LayoutManager&&) = default;
        LayoutManager& operator = (const LayoutManager&) = delete;
        LayoutManager& operator = (LayoutManager&&) = default;

        virtual ~LayoutManager() = default;

        /**
         * @brief Default layout strategy is to do nothing.
         * @param widget The widget to layout.
         * @return true on success, false on fail.
         */
         virtual bool initialWidgetLayout(Context &context, std::shared_ptr<Widget> &widget);
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
