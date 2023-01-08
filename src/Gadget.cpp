//
// Created by richard on 31/12/22.
//

/*
 * Gadget.cpp Created by Richard Buckley (C) 31/12/22
 */

#include "Gadget.h"
#include <Window.h>

namespace rose {
    void Gadget::managedBy(const std::shared_ptr<Widget>& widget) {
        manager = widget;
    }

    void Gadget::draw(Context &context, Point drawLocation) {
        mVisualMetrics.lastDrawLocation = drawLocation;
        for (const auto& decorator : mDecorators) {
            decorator(context, *this);
        }
    }

    bool Gadget::forceInitialGadgetLayout() {

        /**
         * The renderRect size is the Gadget desired size + the Gadget padding.
         */
        mVisualMetrics.renderRect.size = mVisualMetrics.desiredSize + mVisualMetrics.gadgetBorder +
                                         mVisualMetrics.gadgetPadding.topLeft + mVisualMetrics.gadgetPadding.botRight;

        /**
         * The borderRect size is the renderRect size + the boarder size all the way around.
         */
        mVisualMetrics.borderRect.size = mVisualMetrics.renderRect.size + mVisualMetrics.gadgetPadding.topLeft +
                                         mVisualMetrics.gadgetPadding.botRight + mVisualMetrics.innerAlignmentPadding.topLeft +
                                         mVisualMetrics.innerAlignmentPadding.botRight + mVisualMetrics.gadgetBorder * 2;

        /**
         * The clipRectangle size is the borderRect size + the manager padding.
         */
        mVisualMetrics.clipRectangle.size = mVisualMetrics.borderRect.size +
                                            mVisualMetrics.outerAlignmentPadding.topLeft + mVisualMetrics.outerAlignmentPadding.botRight;

        /**
         * The clipRectangle point is 0,0
         */
        mVisualMetrics.clipRectangle.point = Point{0,0};

        /**
         * The borderRect point is the clipRectangle point + manager padding.
         */
        mVisualMetrics.borderRect.point = mVisualMetrics.clipRectangle.point +
                                          mVisualMetrics.outerAlignmentPadding.topLeft;

        /**
         * The renderRect point is the borderRect point + the border size.
         */
        mVisualMetrics.renderRect.point = mVisualMetrics.borderRect.point + mVisualMetrics.gadgetBorder +
                                          mVisualMetrics.gadgetPadding.topLeft + mVisualMetrics.innerAlignmentPadding.topLeft;

        fmt::print("initialGadgetLayout: {}"
                   "\n\tdrawLocation:    {}"
                   "\n\touterPadding:    {}"
                   "\n\tinnerPadding:    {}"
                   "\n\tgadgetPadding:   {}"
                   "\n\tgadgetBoarder:   {}"
                   "\n\tdesiredSize:     {}"
                   "\n\trenderRectangle: {}"
                   "\n\tborderRectangle: {}"
                   "\n\tclipRectangle:   {}\n\n",
                   mName, mVisualMetrics.drawLocation, mVisualMetrics.outerAlignmentPadding, mVisualMetrics.innerAlignmentPadding,
                   mVisualMetrics.gadgetPadding, mVisualMetrics.gadgetBorder, mVisualMetrics.desiredSize, mVisualMetrics.renderRect,
                   mVisualMetrics.borderRect, mVisualMetrics.clipRectangle
        );

        return false;
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
    bool Gadget::enterLeaveEvent(bool enter, Uint32 timestamp) {
        if (isManaged()) {
            return manager.lock()->enterLeaveEvent(enter, timestamp);
        }
        return false;
    }

    bool Gadget::mouseButtonEvent(const SDL_MouseButtonEvent &e) {
        if (isManaged()) {
            return manager.lock()->mouseButtonEvent(e);
        }
        return false;
    }

#pragma clang diagnostic pop

    void backgroundDecorator(Context& context, Gadget& gadget) {
        auto visualMetrics = gadget.getVisualMetrics();
        if (visualMetrics.background) {
            DrawColorGuard colorGuard{context, visualMetrics.background};
            context.fillRect(visualMetrics.borderRect + visualMetrics.drawLocation);
        }
    }

    std::shared_ptr<Screen> Gadget::getScreen() {
        if (isManaged()) {
            for (auto widget = manager.lock(); widget; widget = widget->manager.lock()) {
                if (auto screen = std::dynamic_pointer_cast<Screen>(widget); screen)
                    return screen;
            }
        }

        return {};
    }

    [[maybe_unused]] void Widget::manage(std::shared_ptr<Gadget> gadget) {
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
