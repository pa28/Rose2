//
// Created by richard on 10/01/23.
//

/*
 * Widget.cpp Created by Richard Buckley (C) 10/01/23
 */

/**
 * @file Widget.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 10/01/23
 */

#include "Widget.h"
#include "Gadget.h"

namespace rose {

    [[maybe_unused]] void Widget::manage(std::shared_ptr<Gadget> gadget) {
        gadget->managedBy(std::dynamic_pointer_cast<Widget>(shared_from_this()));
        mGadgetList.push_back(std::move(gadget));
        if (mIsInitialized)
            mGadgetList.back()->initialize();
    }

    void Widget::manage(Builder &builder) {
        builder.gadget->managedBy(std::dynamic_pointer_cast<Widget>(shared_from_this()));
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

    bool Widget::initialLayout(Context &context) {
        if (mLayoutManager) {
            auto widget = shared_from_this();
            return mLayoutManager->initialWidgetLayout(context, widget);
        } else {
            bool constraintRequired = false;
            for (auto &gadget: mGadgetList) {
                constraintRequired |= gadget->initialLayout(context);
            }

            return constraintRequired;
        }
    }

    void Widget::initialize() {
        Gadget::initialize();
        for (auto& gadget : mGadgetList) {
            gadget->initialize();
        }
    }


} // Rose