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
#include "Rose.h"

namespace rose {

/**
 * @class Gadget
 */
    class Widget;

    class Gadget {
    protected:
        constexpr static GadgetType ThisType = GadgetType::Gadget;
        std::shared_ptr<Widget> manager{};

        Rectangle desiredLayout{};
        Rectangle managedLayout{};

    public:
        Gadget() = default;
        Gadget(const Gadget&) = delete;
        Gadget(Gadget&&) = default;
        Gadget& operator = (const Gadget &) = delete;
        Gadget& operator = (Gadget &&) = default;

        [[nodiscard]] virtual GadgetType gadgetType() const { return Gadget::ThisType; }

        void managedBy(std::shared_ptr<Widget> widget) {
            manager = std::move(widget);
        }

        virtual ~Gadget() = default;
    };

    /**
     * @class Widget
     */
    class Widget: public std::enable_shared_from_this<Widget>, Gadget {
    protected:
        constexpr static GadgetType ThisType = GadgetType::Widget;

        std::vector<std::shared_ptr<Gadget>> gadgetList{};

    public:
        Widget() = default;
        Widget(const Widget&) = delete;
        Widget(Widget&&) = default;
        Widget& operator = (const Widget &) = delete;
        Widget& operator = (Widget &&) = default;

        [[nodiscard]] GadgetType gadgetType() const override { return Widget::ThisType; }

        void manage(std::shared_ptr<Gadget> gadget) {
            gadget->managedBy(shared_from_this());
            gadgetList.push_back(std::move(gadget));
        }

        auto begin() { return gadgetList.begin(); }
        auto end() { return gadgetList.end(); }

        ~Widget() override = default;
    };

} // rose

#endif //ROSE2_GADGET_H
