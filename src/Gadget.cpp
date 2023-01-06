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

    void Gadget::draw(Context &context) {
        if (mVisualMetrics.background) {
            DrawColorGuard colorGuard{context, mVisualMetrics.background};
            context.fillRect(mVisualMetrics.clipRectangle);
        }
    }

    Point Gadget::layout(Context &, Rectangle /*constraint*/) {
        return mVisualMetrics.drawLocation;
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

    Point Widget::layout(Context &context, Rectangle constraint) {
        if (mLayoutManager) {
            auto widget = shared_from_this();
            return mLayoutManager->layoutWidget(context, constraint, widget);
        } else {
            if (!constraint) {
                Gadget::layout(context, constraint);

                for (auto &gadget: mGadgetList) {
                    Point drawLocation = gadget->layout(context, constraint);
                    gadget->layoutGadget(drawLocation, Padding{});
                }

                return Point{};
            } else {
                Gadget::layout(context, constraint);

                for (auto &gadget: mGadgetList) {
                    Point drawLocation = gadget->layout(context, constraint);
                    gadget->layoutGadget(drawLocation, Padding{});
                }

                return Point{};
            }
        }
    }

    Point LayoutManager::layoutWidget(Context &context, Rectangle constraint, std::shared_ptr<Widget> &widget) {
        if (!constraint) {
            for (auto &gadget: widget->mGadgetList) {
                auto drawLocation = gadget->layout(context, constraint);
                gadget->layoutGadget(drawLocation, Padding());
            }
        } else {
            for (auto &gadget: widget->mGadgetList) {
                auto drawLocation = gadget->layout(context, constraint);
                gadget->layoutGadget(drawLocation, Padding());
            }
        }

        return Point{};
    }
} // rose
