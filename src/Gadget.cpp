//
// Created by richard on 31/12/22.
//

/*
 * Gadget.cpp Created by Richard Buckley (C) 31/12/22
 */

#include "Gadget.h"

namespace rose {
    void Gadget::managedBy(std::shared_ptr<Widget> widget) {
        manager = std::move(widget);
    }

    void Gadget::draw(Context &context) {
        DrawColorGuard colorGuard{context, background};
        context.fillRect(desiredLayout);
    }

    void Gadget::layout(Context &) {

    }

    [[maybe_unused]] void Widget::manage(std::shared_ptr<Gadget> gadget) {
        if (auto manager = gadget->getManager(); manager) {
            manager->unManage(gadget);
        }
        gadget->managedBy(shared_from_this());
        mGadgetList.push_back(std::move(gadget));
    }

    void Widget::manage(Builder &builder) {
        builder.gadget->managedBy(shared_from_this());
        mGadgetList.push_back(std::move(builder.gadget));
    }

    [[maybe_unused]] void Widget::unManage(const std::shared_ptr<Gadget> &gadget) {
        mGadgetList.erase(std::remove(mGadgetList.begin(), mGadgetList.end(), gadget), mGadgetList.end());
    }

    void Widget::draw(Context &context) {
        Gadget::draw(context);

        for (const auto& gadget : mGadgetList) {
            gadget->draw(context);
        }
    }

    void Widget::layout(Context &context) {
        Gadget::layout(context);

        for (auto &gadget : mGadgetList) {
            gadget->layout(context);
        }
    }

    void Builder::operator>>(Widget &widget) {
        widget.manage(*this);
    }

    void Builder::operator>>(std::shared_ptr<Widget> &widget) {
        widget->manage(*this);
    }

    void Builder::operator>>(Builder &builder) {
        if (auto ptr = std::dynamic_pointer_cast<Widget>(builder.gadget); ptr) {
            ptr->manage(*this);
        }
    }
} // rose