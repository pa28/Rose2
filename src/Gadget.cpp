
/*
 * Gadget.cpp Created by Richard Buckley (C) 31/12/22
 */

/**
 * @file Gadget.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 031/12/22
 */

#include "Gadget.h"
#include "manager/Widget.h"
#include "manager/Singlet.h"
#include "manager/Window.h"
#include <Application.h>
#include <Animation.h>

namespace rose {

    std::weak_ptr<Application> Gadget::mApplicationPtr;

    void Gadget::managedBy(const std::shared_ptr<Gadget> &gadget) {
        if (auto singlet = std::dynamic_pointer_cast<Singlet>(gadget); singlet)
            manager = singlet;
        else if (auto widget = std::dynamic_pointer_cast<Widget>(gadget); widget)
            manager = widget;
        else
            throw SceneTreeError{"Manager is not derived from rose::Singlet."};
    }

    void Gadget::draw(Context &context, Point drawLocation) {
        mVisualMetrics.lastDrawLocation = drawLocation;
        if (!mDecorators.empty()) {
            for (const auto &decorator: mDecorators) {
                decorator(context, *this);
            }
        } else {
            if (mVisualMetrics.background) {
                context.fillRect(mVisualMetrics.clipRectangle + drawLocation, mVisualMetrics.background);
            }
            if (mVisualMetrics.animateBackground) {
                context.fillRect(mVisualMetrics.clipRectangle + drawLocation, mVisualMetrics.animateBackground);
            }
        }
        mNeedsDrawing = false;
    }

    void Gadget::expose(Context &context, Rectangle exposed) {
        auto lastDrawn = mVisualMetrics.lastDrawLocation;
        if (auto exposedGadget = (mVisualMetrics.clipRectangle + mVisualMetrics.lastDrawLocation).intersection(exposed); exposedGadget) {
            ClipRectangleGuard clipRectangleGuard{context, exposedGadget};
            draw(context, mVisualMetrics.lastDrawLocation);
        }
    }

    bool Gadget::immediateGadgetLayout() {

        /**
         * The renderRect size is the Gadget desired size + the Gadget padding.
         */
        mVisualMetrics.renderRect.size = mVisualMetrics.desiredSize;
        /**
         * The clipRectangle size is the borderRect size + the manager padding.
         */
        mVisualMetrics.clipRectangle.size = mVisualMetrics.renderRect.size +
                mVisualMetrics.gadgetPadding.topLeft + mVisualMetrics.gadgetPadding.botRight +
                mVisualMetrics.innerAlignmentPadding.topLeft + mVisualMetrics.innerAlignmentPadding.botRight;

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

    void Gadget::setNeedsLayout() {
        mNeedsLayout = true;
        if (auto screenPtr = std::dynamic_pointer_cast<Screen>(shared_from_this()); screenPtr) {
            screenPtr->getScreenWindow().lock()->setNeedsLayout();
        } else if (auto screen = getScreen(); screen) {
            screen->setNeedsLayout();
        }
    }

    void Gadget::setNeedsDrawing() {
        mNeedsDrawing = true;
        if (auto screenPtr = std::dynamic_pointer_cast<Screen>(shared_from_this()); screenPtr) {
            screenPtr->getScreenWindow().lock()->setNeedsDrawing();
        } else if (auto screen = getScreen(); screen){
            screen->setNeedsDrawing();
        }
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

    std::shared_ptr<Application> Gadget::getApplicationPtr() {
        if (mApplicationPtr.expired()) {
            mApplicationPtr = getScreen()->getApplication();
        }
        return mApplicationPtr.lock();
    }

    std::shared_ptr<Theme> Gadget::getTheme() {
        if (mApplicationPtr.expired()) {
            getApplicationPtr();
        }
        return mApplicationPtr.lock()->getTheme();
    }

    std::shared_ptr<Theme> Gadget::getThemeValues() {
        auto theme = getTheme();
        return theme;
    }

    Gadget::Gadget() {

    }

    std::shared_ptr<Window> Gadget::getWindow() {
        if (auto screen = getScreen(); screen) {
            if (auto window = screen->getScreenWindow(); !window.expired()) {
                return window.lock();
            }
        }
        return nullptr;
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

    [[maybe_unused]] void ThemeBackgroundDecorator(Context &context, Gadget &gadget) {
        auto visualMetrics = gadget.getVisualMetrics();
        auto theme = gadget.getTheme();
        context.fillRect(visualMetrics.renderRect + visualMetrics.drawLocation,
                         theme->colorShades[ThemeColor::Base]);
    }
} // rose
