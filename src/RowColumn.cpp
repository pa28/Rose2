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
#include <memory>

namespace rose {
    bool LinearLayout::initialWidgetLayout(Context &context, std::shared_ptr<Widget> &widget) {
        bool constraintLayoutRequired = false;

        Point position{0,0};
        clearSizes();
        for (auto &gadget : getGadgetList(widget)) {
            constraintLayoutRequired |= gadget->initialGadgetLayout(context);
            gadget->getVisualMetrics().drawLocation = position;
            switch (mMajorAxis) {
                case MajorAxis::HORIZONTAL:
                    mMajorAxisSize += gadget->getVisualMetrics().clipRectangle.size.w;
                    mMinorAxisMax = std::max(mMinorAxisMax, gadget->getVisualMetrics().clipRectangle.size.h);
                    position.x += gadget->getVisualMetrics().clipRectangle.size.w;
                    break;
                case MajorAxis::VERTICAL:
                    mMajorAxisSize += gadget->getVisualMetrics().clipRectangle.size.h;
                    mMinorAxisMax = std::max(mMinorAxisMax, gadget->getVisualMetrics().clipRectangle.size.w);
                    position.y += gadget->getVisualMetrics().clipRectangle.size.h;
                    break;
            }
        }

        for (auto &gadget : getGadgetList(widget)) {
            gadget->getVisualMetrics().innerAlignmentPadding = Padding{};
            gadget->getVisualMetrics().outerAlignmentPadding = Padding{};

            switch (mMajorAxis) {
                case MajorAxis::HORIZONTAL:
                    if (auto residue = mMinorAxisMax - gadget->getVisualMetrics().clipRectangle.size.h; residue > 0) {
                        Padding padding{0,0,0,0};

                        switch (mAlignment) {
                            case Alignment::TOP_LEFT:
                                padding.botRight.y = residue;
                                if (mAlignmentLoc == AlignmentLoc::INNER) {
                                    gadget->getVisualMetrics().innerAlignmentPadding = padding;
                                } else {
                                    gadget->getVisualMetrics().outerAlignmentPadding = padding;
                                }
                                break;
                            case Alignment::BOTTOM_RIGHT:
                                padding.topLeft.y = residue;
                                if (mAlignmentLoc == AlignmentLoc::INNER) {
                                    gadget->getVisualMetrics().innerAlignmentPadding = padding;
                                } else {
                                    gadget->getVisualMetrics().outerAlignmentPadding = padding;
                                }
                                break;
                            case Alignment::CENTER:
                                padding.topLeft.y = residue / 2;
                                padding.botRight.y = residue / 2 + (residue % 2 ? 1 : 0);
                                if (mAlignmentLoc == AlignmentLoc::INNER) {
                                    gadget->getVisualMetrics().innerAlignmentPadding = padding;
                                } else {
                                    gadget->getVisualMetrics().outerAlignmentPadding = padding;
                                }
                                break;
                        }
                        gadget->forceInitialGadgetLayout();
                    }
                    break;
                case MajorAxis::VERTICAL:
                    if (auto residue = mMinorAxisMax - gadget->getVisualMetrics().clipRectangle.size.w; residue > 0) {
                        Padding padding{0,0,0,0};

                        switch (mAlignment) {
                            case Alignment::TOP_LEFT:
                                padding.botRight.x = residue;
                                if (mAlignmentLoc == AlignmentLoc::INNER) {
                                    gadget->getVisualMetrics().innerAlignmentPadding = padding;
                                } else {
                                    gadget->getVisualMetrics().outerAlignmentPadding = padding;
                                }
                                break;
                            case Alignment::BOTTOM_RIGHT:
                                padding.topLeft.x = residue;
                                if (mAlignmentLoc == AlignmentLoc::INNER) {
                                    gadget->getVisualMetrics().innerAlignmentPadding = padding;
                                } else {
                                    gadget->getVisualMetrics().outerAlignmentPadding = padding;
                                }
                                break;
                            case Alignment::CENTER:
                                padding.topLeft.x = residue / 2;
                                padding.botRight.x = residue / 2 + (residue % 2 ? 1 : 0);
                                if (mAlignmentLoc == AlignmentLoc::INNER) {
                                    gadget->getVisualMetrics().innerAlignmentPadding = padding;
                                } else {
                                    gadget->getVisualMetrics().outerAlignmentPadding = padding;
                                }
                                break;
                        }
                        gadget->forceInitialGadgetLayout();
                    }
                    break;
            }
        }

        switch (mMajorAxis) {
            case MajorAxis::HORIZONTAL:
                widget->getVisualMetrics().desiredSize = Size{mMajorAxisSize, mMinorAxisMax };
                break;
            case MajorAxis::VERTICAL:
                widget->getVisualMetrics().desiredSize = Size{ mMinorAxisMax, mMajorAxisSize };
                break;
        }
        constraintLayoutRequired |= widget->forceInitialGadgetLayout();

        return constraintLayoutRequired;
    }
} // rose