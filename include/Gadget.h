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
#include "Rose.h"

namespace rose {

/**
 * @class Gadget
 * @brief A Gadget is a visual UI element that does not manage any other elements.
 */
    class Widget;

    class Gadget {
    protected:
        constexpr static GadgetType ThisType = GadgetType::Gadget;
        std::shared_ptr<Widget> manager{};  ///< Pointer to the current manager of this Gadget.

        [[maybe_unused]] Rectangle desiredLayout{};          ///< The desired layout set by the Gadget or constructor.
        [[maybe_unused]] Rectangle managedLayout{};          ///< The layout set by the manager if it provides layout management.

    public:
        Gadget() = default;
        Gadget(const Gadget&) = delete;
        Gadget(Gadget&&) = default;
        Gadget& operator = (const Gadget &) = delete;
        Gadget& operator = (Gadget &&) = default;

        [[nodiscard]] virtual GadgetType gadgetType() const { return Gadget::ThisType; }

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


        virtual ~Gadget() = default;
    };

    /**
     * @class Widget
     * @brief A Widget is a visual UI element that does manage other Gadgets.
     * @details The list of managed Gadgets is maintained in a std::vector. Many
     * std::vector methods are exposed allowing colling programs to utilize
     * these methods and range based algorithms.
     */
    class Widget: public std::enable_shared_from_this<Widget>, Gadget {
    protected:
        constexpr static GadgetType ThisType = GadgetType::Widget;

        std::vector<std::shared_ptr<Gadget>> gadgetList{};  ///< The list of Gadgets managed.

    public:
        Widget() = default;
        Widget(const Widget&) = delete;
        Widget(Widget&&) = default;
        Widget& operator = (const Widget &) = delete;
        Widget& operator = (Widget &&) = default;

        [[nodiscard]] GadgetType gadgetType() const override { return Widget::ThisType; }

        /**
         * @brief Add a Gadget to the management list of this Widget.
         * @details This will include setting this Widget as the manager of the Gadget, which will remove the
         * gadget from the management list of its current manager, if any.
         * @param gadget The Gadget to manage.
         */
        void manage(std::shared_ptr<Gadget> gadget);

        /**
         * @brief Get a pointer to the first Gadget on the list of managed Gadgets.
         * @details This does not un-manage the Gadget.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto front() { return gadgetList.front(); }

        /**
         * @brief Get a pointer to the last Gadget on the list of managed Gadgets.
         * @details This does not un-manage the Gadget.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto back() { return gadgetList.back(); }

        /**
         * @brief Get a pointer to the Gadget at idx on the list of managed Gadgets.
         * @details This does not un-manage the Gadget. Access is by calling vector::at(idx).
         * @param idx The index of the Gadget to return.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto at(size_t idx) { return gadgetList.at(idx); }

        /**
         * @brief Get a pointer to the Gadget at idx on the list of managed Gadgets.
         * @details This does not un-manage the Gadget. Access is by calling vector::operator[](idx).
         * @param idx The index of the Gadget to return.
         * @return std::shared_ptr<Gadget>
         */
        [[maybe_unused]] auto operator[](size_t idx) { return gadgetList.operator[](idx); }

        /**
         * @brief Get the count of Gadgets managed by this Widget by calling vector::size().
         * @return The number of Gadgets.
         */
        [[maybe_unused]] auto gadgetCount() const { return gadgetList.size(); }

        /**
         * @brief Un-manage the Gadget if it is found by std::remove.
         * @param gadget The gadget to remove.
         */
        void unManage(const std::shared_ptr<Gadget>& gadget);

        /**
         * @brief Get the begin iterator of the Gadget list.
         * @return The return value of vector::begin().
         */
        auto begin() { return gadgetList.begin(); }

        /**
         * @brief Get the begin iterator of the Gadget list.
         * @return The return value of vector::end().
         */
        auto end() { return gadgetList.end(); }

        ~Widget() override = default;
    };

} // rose

#endif //ROSE2_GADGET_H
