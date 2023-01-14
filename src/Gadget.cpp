//
// Created by richard on 31/12/22.
//

/*
 * Gadget.cpp Created by Richard Buckley (C) 31/12/22
 */

#include "Gadget.h"
#include "Widget.h"
#include "Singlet.h"
#include <Window.h>

namespace rose {
    void Gadget::managedBy(const std::shared_ptr<Gadget> &gadget) {
        if (auto singlet = std::dynamic_pointer_cast<Singlet>(gadget); singlet)
            manager = singlet;
        else if (auto widget = std::dynamic_pointer_cast<Widget>(gadget); widget)
            manager = widget;
        else
            throw SceneTreeError{"Manager is not derived from rose::Singlet."};
    }

    void Gadget::draw(Context &context, Point drawLocation) {
        mVisualMetrics.drawLocation = drawLocation;
        for (const auto& decorator : mDecorators) {
            decorator(context, *this);
        }

        if (mVisualMetrics.background) {
            context.fillRect(mVisualMetrics.renderRect + drawLocation, mVisualMetrics.background);
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
        mVisualMetrics.clipRectangle.size = mVisualMetrics.renderRect.size + mVisualMetrics.gadgetPadding.botRight +
                mVisualMetrics.gadgetPadding.topLeft;

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
                   "\n\tinnerPadding:    {}"
                   "\n\tgadgetPadding:   {}"
                   "\n\tdesiredSize:     {}"
                   "\n\trenderRectangle: {}"
                   "\n\tclipRectangle:   {}\n\n",
                   mName, mVisualMetrics.background, mVisualMetrics.drawLocation, mVisualMetrics.innerAlignmentPadding,
                   mVisualMetrics.gadgetPadding, mVisualMetrics.desiredSize, mVisualMetrics.renderRect,
                   mVisualMetrics.clipRectangle
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

    void Builder::operator>>(Singlet &singlet) {
        singlet.manage(*this);
    }

    void Builder::operator>>(const std::shared_ptr<Widget>& widget) {
        widget->manage(*this);
    }

    void Builder::operator>>(const std::shared_ptr<Singlet> &singlet) {
        singlet->manage(*this);
    }

    void Builder::operator>>(Builder &builder) {
        if (auto widget = std::dynamic_pointer_cast<Widget>(builder.gadget); widget) {
            widget->manage(*this);
        } else if (auto singlet = std::dynamic_pointer_cast<Singlet>(builder.gadget); singlet) {
            singlet->manage(*this);
        }
    }

    bool LayoutManager::initialWidgetLayout(Context &context, std::shared_ptr<Gadget> &gadget) {
        bool result = false;
        if (auto widget = std::dynamic_pointer_cast<Widget>(gadget)) {
            for (auto &managed: getGadgetList(widget)) {
                result |= managed->initialLayout(context);
            }
        } else if (auto singlet = std::dynamic_pointer_cast<Singlet>(gadget)) {
            if (auto managed = getGadget(singlet))
                result |= managed->initialLayout(context);
        }

        return result;
    }
} // rose
