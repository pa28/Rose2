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
            clearSizes();
            for (auto &gadget: getGadgetList(widget)) {
                auto drawLocation = gadget->layout(context, constraint);
                gadget->layoutGadget(drawLocation, Padding());
                auto visualMetrics = gadget->getVisualMetrics();
                switch (mMajorAxis) {
                    case MajorAxis::VERTICAL:
                        mMajorAxisSize += visualMetrics.clipRectangle.size.h;
                        mMinorAxisMax = std::max(mMinorAxisMax, visualMetrics.clipRectangle.size.w);
                        break;
                    case MajorAxis::HORIZONTAL:
                        mMajorAxisSize += visualMetrics.clipRectangle.size.w;
                        mMinorAxisMax = std::max(mMinorAxisMax, visualMetrics.clipRectangle.size.h);
                        break;
                }
            }
            switch (mMajorAxis) {
                case MajorAxis::VERTICAL:
                    widget->getVisualMetrics().desiredSize = Size(mMinorAxisMax, mMajorAxisSize);
                    break;
                case MajorAxis::HORIZONTAL:
                    widget->getVisualMetrics().desiredSize = Size(mMajorAxisSize, mMinorAxisMax);
                    break;
            }

        } else {
            Point placement = widget->getVisualMetrics().renderRect.point;
            widget->getVisualMetrics().desiredSize = Size{0,0};
            for (auto &gadget: getGadgetList(widget)) {
                gadget->layoutGadget(placement, Padding());
                auto visualMetrics = gadget->getVisualMetrics();
                switch (mMajorAxis) {
                    case MajorAxis::VERTICAL:
                        placement.y += visualMetrics.clipRectangle.size.h;
                        widget->getVisualMetrics().desiredSize.h = std::max(widget->getVisualMetrics().desiredSize.h,
                                                                             visualMetrics.clipRectangle.size.h);
                        widget->getVisualMetrics().desiredSize.w = std::max(widget->getVisualMetrics().desiredSize.w,
                                                                            visualMetrics.clipRectangle.size.w);
                        break;
                    case MajorAxis::HORIZONTAL:
                        placement.x += visualMetrics.clipRectangle.size.w;
                        widget->getVisualMetrics().desiredSize.w = std::max(widget->getVisualMetrics().desiredSize.w,
                                                                             visualMetrics.clipRectangle.size.w);
                        widget->getVisualMetrics().desiredSize.h = std::max(widget->getVisualMetrics().desiredSize.h,
                                                                            visualMetrics.clipRectangle.size.h);
                        break;
                }

                gadget->layoutGadget(visualMetrics.drawLocation, Padding());
            }

        }

        return Point{};
    }
} // rose