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
        return false;
    }

    void Singlet::manage(std::shared_ptr<Gadget> gadget) {
        gadget->managedBy(shared_from_this());
        mGadget = std::move(gadget);
    }

    void Singlet::manage(Builder &builder) {
        builder.gadget->managedBy(std::dynamic_pointer_cast<Singlet>(shared_from_this()));
        mGadget = std::move(builder.gadget);
    }

    void Singlet::draw(Context &context, Point drawLocation) {
        if (mGadget)
            mGadget->draw(context, drawLocation);
    }

} // rose