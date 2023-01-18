//
// Created by richard on 10/01/23.
//

/*
 * Singlet.cpp Created by Richard Buckley (C) 10/01/23
 */

/**
 * @file Singlet.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 10/01/23
 */

#include "Singlet.h"

namespace rose {
    bool Singlet::initialLayout(Context &context) {
        if (mGadget) {
            mGadget->initialLayout(context);
            mVisualMetrics.desiredSize = mGadget->getVisualMetrics().clipRectangle.size;
            Gadget::initialLayout(context);
        }

//        debugLayout(__PRETTY_FUNCTION__ );
        return false;
    }

    void Singlet::setInternalAlignmentPadding(const Padding &padding) {
        if (mGadget) {
            mGadget->setInternalAlignmentPadding(padding);
            mGadget->immediateGadgetLayout();
            mVisualMetrics.desiredSize = mGadget->getVisualMetrics().clipRectangle.size;
            Gadget::immediateGadgetLayout();
//            debugLayout(__PRETTY_FUNCTION__ );
        }
    }

    [[maybe_unused]] void Singlet::manage(std::shared_ptr<Gadget> gadget) {
        gadget->managedBy(shared_from_this());
        mGadget = std::move(gadget);
    }

    void Singlet::manage(Builder &builder) {
        builder.gadget->managedBy(std::dynamic_pointer_cast<Singlet>(shared_from_this()));
        mGadget = std::move(builder.gadget);
    }

    void Singlet::draw(Context &context, Point drawLocation) {
        Gadget::draw(context, drawLocation);
        if (mGadget) {
            mGadget->draw(context, drawLocation + mVisualMetrics.renderRect.point);
        }
    }

    bool Singlet::immediateGadgetLayout() {
        mGadget->immediateGadgetLayout();
        Gadget::immediateGadgetLayout();
        return false;
    }

    [[maybe_unused]] void Singlet::debugLayout(const char *func) {
        auto vm = mGadget->getVisualMetrics();
        fmt::print("{} {}\n", func, mGadget->name());
        fmt::print("\tdrawLocation: {}\n", mVisualMetrics.drawLocation);
        fmt::print("\tdesiredSize: {}\n", mVisualMetrics.desiredSize);
        fmt::print("\tinnerAlignment: {}\n", mVisualMetrics.innerAlignmentPadding);
        fmt::print("\tgadgetPadding: {}\n", mVisualMetrics.gadgetPadding);
        fmt::print("\trenderRect: {}\n", mVisualMetrics.renderRect);
        fmt::print("\tclipRectangle: {}\n", mVisualMetrics.clipRectangle);
        fmt::print("\t\tdrawLocation: {}\n", vm.drawLocation);
        fmt::print("\t\tdesiredSize: {}\n", vm.desiredSize);
        fmt::print("\t\tinnerAlignment: {}\n", vm.innerAlignmentPadding);
        fmt::print("\t\tgadgetPadding: {}\n", vm.gadgetPadding);
        fmt::print("\t\trenderRect: {}\n", vm.renderRect);
        fmt::print("\t\tclipRectangle: {}\n", vm.clipRectangle);
    }

} // rose