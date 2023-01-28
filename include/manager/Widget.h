//
// Created by richard on 10/01/23.
//

/*
 * Widget.h Created by Richard Buckley (C) 10/01/23
 */

/**
 * @file Widget.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 10/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_WIDGET_H
#define ROSE2_WIDGET_H

#include <memory>
#include "Singlet.h"

namespace rose {

    /**
     * @class Widget
     * @brief A Widget is a visual UI element that does manage other Gadgets.
     * @details The list of managed Gadgets is maintained in a std::vector. Many
     * std::vector methods are exposed allowing colling programs to utilize
     * these methods and range based algorithms.
     */
    class Widget: public Gadget {
        friend class LayoutManager;
        friend class Window;

    protected:
        constexpr static std::string_view ClassName = "Widget";

        std::vector<std::shared_ptr<Gadget>> mGadgetList{};  ///< The list of Gadgets managed.

        std::shared_ptr<LayoutManager> mLayoutManager{};

    public:
        Widget() = default;
        explicit Widget(std::shared_ptr<Theme>& theme) : Gadget(theme) {}
        Widget(const Widget&) = delete;
        Widget(Widget&&) = default;
        Widget& operator = (const Widget &) = delete;
        Widget& operator = (Widget &&) = default;

        void initialize() override;

        bool initialLayout(Context &context) override;

        template<class Layout>
        requires std::derived_from<LayoutManager,Layout>
        void setLayoutManager(std::unique_ptr<Layout>&& layout) {
            mLayoutManager = std::move(layout);
        }

        const std::string_view& className() const override { return Widget::ClassName; }

        /**
         * @brief Add a Gadget to the management list of this Widget.
         * @details This will include setting this Widget as the manager of the Gadget, which will remove the
         * gadget from the management list of its current manager, if any.
         * @param gadget The Gadget to manage.
         * @throws SceneTreeError
         */
        [[maybe_unused]] virtual void manage(std::shared_ptr<Gadget> gadget);

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

        void expose(Context &context, Rectangle exposed) override;

        ~Widget() override = default;
    };

    class LayoutManager {
    protected:
        static auto getGadgetList(std::shared_ptr<Widget> &widget) {
            return widget->mGadgetList;
        }

        static auto getGadget(std::shared_ptr<Singlet> &singlet) {
            return singlet->mGadget;
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
         * @param gadget The widget to layout.
         * @return true on success, false on fail.
         */
        virtual bool initialWidgetLayout(Context &context, std::shared_ptr<Gadget> &gadget);
    };

} // rose

#endif //ROSE2_WIDGET_H
