//
// Created by richard on 31/12/22.
//

/*
 * Gadget.cpp Created by Richard Buckley (C) 31/12/22
 */

#include "Gadget.h"
#include "Widget.h"
#include <Window.h>
#include <exception>

namespace rose {
    void Gadget::managedBy(const std::shared_ptr<Gadget> &gadget) {
        if (std::dynamic_pointer_cast<Singlet>(gadget))
            manager = gadget;
        else if (std::dynamic_pointer_cast<Widget>(gadget))
            manager = gadget;
        else
            throw SceneTreeError{"Manager is not derived from rose::Singlet."};
    }

    void Gadget::draw(Context &context, Point drawLocation) {
        mVisualMetrics.drawLocation = drawLocation;
        for (const auto& decorator : mDecorators) {
            decorator(context, *this);
        }
    }

    bool Gadget::forceInitialGadgetLayout() {

        /**
         * The renderRect size is the Gadget desired size + the Gadget padding.
         */
        mVisualMetrics.renderRect.size = mVisualMetrics.desiredSize +
                                         mVisualMetrics.gadgetPadding.topLeft + mVisualMetrics.gadgetPadding.botRight;

        /**
         * The clipRectangle size is the borderRect size + the manager padding.
         */
        mVisualMetrics.clipRectangle.size = mVisualMetrics.renderRect.size;

        /**
         * The clipRectangle point is 0,0
         */
        mVisualMetrics.clipRectangle.point = Point{0,0};

        /**
         * The renderRect point is the borderRect point + the border size.
         */
        mVisualMetrics.renderRect.point = mVisualMetrics.gadgetPadding.topLeft + mVisualMetrics.innerAlignmentPadding.topLeft;

#if 0
        fmt::print("initialGadgetLayout: {}"
                   "\n\tbackgroundColor: {}"
                   "\n\tdrawLocation:    {}"
                   "\n\touterPadding:    {}"
                   "\n\tinnerPadding:    {}"
                   "\n\tgadgetPadding:   {}"
                   "\n\tgadgetBoarder:   {}"
                   "\n\tdesiredSize:     {}"
                   "\n\trenderRectangle: {}"
                   "\n\tborderRectangle: {}"
                   "\n\tclipRectangle:   {}\n\n",
                   mName, mVisualMetrics.background, mVisualMetrics.drawLocation, mVisualMetrics.outerAlignmentPadding, mVisualMetrics.innerAlignmentPadding,
                   mVisualMetrics.gadgetPadding, mVisualMetrics.gadgetBorder, mVisualMetrics.desiredSize, mVisualMetrics.renderRect,
                   mVisualMetrics.borderRect, mVisualMetrics.clipRectangle
        );
#endif

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

    std::shared_ptr<Screen> Gadget::getScreen() {
        if (isManaged()) {
            for (auto widget = manager.lock(); widget; widget = widget->manager.lock()) {
                if (auto screen = std::dynamic_pointer_cast<Screen>(widget); screen)
                    return screen;
            }
        }

        return {};
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

    bool LayoutManager::initialWidgetLayout(Context &context, std::shared_ptr<Gadget> &gadget) {
        bool result = false;
        if (auto widget = std::dynamic_pointer_cast<Widget>(gadget)) {
            for (auto &managed: getGadgetList(widget)) {
                result |= managed->initialLayout(context);
            }
        } else if (auto singlet = std::dynamic_pointer_cast<Singlet>(gadget)) {
            auto managed = getGadget(singlet);
        }

        return result;
    }
} // rose
