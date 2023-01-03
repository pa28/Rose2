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

namespace rose {

/**
 * @class Gadget
 * @brief A Gadget is a visual UI element that does not manage any other elements.
 */
    class Widget;

    class Gadget {
    protected:
        friend class GadgetBuilder;
        friend class Window;

        constexpr static GadgetType ThisType = GadgetType::Gadget;
        [[maybe_unused]] std::string_view mName{};
        std::shared_ptr<Widget> manager{};  ///< Pointer to the current manager of this Gadget.

        [[maybe_unused]] Rectangle desiredLayout{};     ///< The desired layout set by the Gadget or constructor.
        [[maybe_unused]] Rectangle managedLayout{};     ///< The layout set by the manager if it provides layout management.
        [[maybe_unused]] Color background{};            ///< The background color.
        [[maybe_unused]] bool mHasFocus{};              ///< This gadget (and managers) has focus.

    public:
        Gadget() = default;
        Gadget(const Gadget&) = delete;
        Gadget(Gadget&&) = default;
        Gadget& operator = (const Gadget &) = delete;
        Gadget& operator = (Gadget &&) = default;

        [[maybe_unused]] [[nodiscard]] virtual GadgetType gadgetType() const { return Gadget::ThisType; }

        /**
         * @brief Set the pointer to the current manager of this Gadget.
         * @param widget
         */
        void managedBy(std::shared_ptr<Widget> widget);

        /**
         * @brief Get the current manager of this Gadget if any.
         * @return A pointer (possibly empty) to the Gadget's manager.
         */
        auto getManager() { return manager; }

        /**
         * @brief Draw this Gadget.
         * @param context The graphics context to use.
         */
        virtual void draw(Context& context);

        /**
         * @brief Layout this Gadget.
         * @details The process of layout is to find the minimum size the Gadget requires to be fully displayed.
         */
        virtual void layout(Context &context);

        virtual ~Gadget() = default;
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

        /**
         * @brief Manage the built Gadget by the specified Widget.
         * @param widget the manager.
         */
        void operator >> (Widget &widget);

        /**
         * @brief Manage the built Gadget by the specified Widget.
         * @param widget a std::shared_ptr to the manager.
         */
        void operator >> (std::shared_ptr<Widget> &widget);

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
            gadget->desiredLayout = rectangle;
            return *this;
        }

        /**
         * @brief Set the desired layout rectangle from a Point and Size.
         * @param point The Point.
         * @param size The Size.
         * @return this builder.
         */
        [[maybe_unused]] auto layout(const Point &point, const Size &size) {
            gadget->desiredLayout = Rectangle(point, size);
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
            gadget->desiredLayout = Rectangle(X, Y, W, H);
            return *this;
        }

        [[maybe_unused]] auto background(const Color& color) {
            gadget->background = color;
            return *this;
        }
    };

    class LayoutManager {
    private:

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
        [[maybe_unused]] virtual bool layoutWidget(std::shared_ptr<Widget> &) { return true; }
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

        void layout(Context &context) override;

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
        void draw(Context& context) override;

        ~Widget() override = default;
    };

    class WidgetBuilder : public GadgetBuilder {
    public:
        [[maybe_unused]] explicit WidgetBuilder(std::shared_ptr<Widget> w) : GadgetBuilder(std::move(w)) {}

        WidgetBuilder() : GadgetBuilder(std::make_shared<Widget>()) {}

        ~WidgetBuilder() override = default;
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
