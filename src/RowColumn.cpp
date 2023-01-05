//
// Created by richard on 04/01/23.
//

/*
 * RowColumn.cpp Created by Richard Buckley (C) 04/01/23
 */

/**
 * @file RowColumn.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 04/01/23
 */

#include "RowColumn.h"

namespace rose {
    Point LinearLayout::layoutWidget(Context &context, Rectangle constraint, std::shared_ptr<Widget> &widget) {
        if (!constraint) {
            for (auto &gadget: getGadgetList(widget)) {
                auto drawLocation = gadget->layout(context, constraint);
                gadget->layoutGadget(drawLocation, Padding());
            }
        } else {
            for (auto &gadget: getGadgetList(widget)) {
                auto drawLocation = gadget->layout(context, constraint);
                gadget->layoutGadget(drawLocation, Padding());
            }
        }

        return Point{};
    }
} // rose