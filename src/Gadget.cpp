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
        context.fillRect(clipRectangle);
    }

    Point Gadget::layout(Context &) {
        return mgrDrawLoc;
    }

    [[maybe_unused]] void Widget::manage(std::shared_ptr<Gadget> gadget) {
        if (isManaged()) {
            manager.lock()->unManage(gadget);
        }
        manager = shared_from_this();
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

    Point Widget::layout(Context &context) {
        Gadget::layout(context);

        for (auto &gadget : mGadgetList) {
            Point drawLocation = gadget->layout(context);
            gadget->layoutGadget(drawLocation, Padding{});
        }

        return Point{};
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