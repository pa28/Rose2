//
// Created by richard on 31/12/22.
//

/*
 * Gadget.cpp Created by Richard Buckley (C) 31/12/22
 */

#include "Gadget.h"

namespace rose {
    void Gadget::managedBy(const std::shared_ptr<Widget>& widget) {
        manager = widget;
    }

    void Gadget::draw(Context &context, Point drawLocation) {
#ifdef DEBUG_GADGET_DRAW
        DrawColorGuard colorGuard{context, ClipRectangleDebugColor};
        context.fillRect(mVisualMetrics.clipRectangle + drawLocation);
        colorGuard.setDrawColor(BorderRectangleDebugColor);
        context.fillRect(mVisualMetrics.borderRect);
        colorGuard.setDrawColor(RenderRectangleDebugColor);
        context.fillRect(mVisualMetrics.renderRect + drawLocation);
#else
        if (mVisualMetrics.background) {
            DrawColorGuard colorGuard{context, mVisualMetrics.background};
            context.fillRect(mVisualMetrics.borderRect + drawLocation);
        }
#endif
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

    void Widget::draw(Context &context, Point drawLocation) {
        Gadget::draw(context, drawLocation);

        for (const auto& gadget : mGadgetList) {
            Point gadgetDrawLocation = drawLocation + gadget->getVisualMetrics().drawLocation;
            gadget->draw(context, gadgetDrawLocation);
        }
    }

    void Builder::operator>>(Widget &widget) {
        widget.manage(*this);
    }

    void Builder::operator>>(const std::shared_ptr<Widget>& widget) {
        widget->manage(*this);
    }

    void Builder::operator>>(Builder &builder) {
        if (auto ptr = std::dynamic_pointer_cast<Widget>(builder.gadget); ptr) {
            ptr->manage(*this);
        }
    }

    bool Widget::initialGadgetLayout(Context &context) {
        if (mLayoutManager) {
            auto widget = shared_from_this();
            return mLayoutManager->initialWidgetLayout(context, widget);
        } else {
            bool constraintRequired = false;
            for (auto &gadget: mGadgetList) {
                constraintRequired |= gadget->initialGadgetLayout(context);
            }

            return constraintRequired;
        }
    }

    bool LayoutManager::initialWidgetLayout(Context &context, std::shared_ptr<Widget> &widget) {
        bool constraintRequired = false;
        for (auto &gadget: getGadgetList(widget)) {
            constraintRequired |= gadget->initialGadgetLayout(context);
        }

        return constraintRequired;
    }
} // rose
