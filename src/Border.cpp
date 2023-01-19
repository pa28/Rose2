//
// Created by richard on 09/01/23.
//

/*
 * Border.cpp Created by Richard Buckley (C) 09/01/23
 */

/**
 * @file Border.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 09/01/23
 */

#include "Border.h"
#include <Theme.h>
#include <GraphicsModel.h>

std::shared_ptr<rose::Theme> rose::Border::getThemeValues() {
    if (auto theme = Gadget::getThemeValues(); theme) {
        mVisual = theme->visual;
        mCorners = theme->corners;
        mVisualMetrics.gadgetPadding = theme->borderSize;
        mVisualMetrics.background = theme->colorShades[ThemeColor::Bottom];
        return theme;
    }
    return nullptr;
}

bool rose::Border::initialLayout(rose::Context &context) {
    getThemeValues();
    return Singlet::initialLayout(context);
}

bool rose::Border::immediateGadgetLayout() {
    return Singlet::immediateGadgetLayout();
}

void rose::Border::draw(rose::Context &context, rose::Point drawLocation) {
    Singlet::draw(context, drawLocation);

    auto borderRect = mVisualMetrics.clipRectangle + drawLocation;
    auto borderSize = mVisualMetrics.gadgetPadding.topLeft.x;
    auto top = getTheme()->colorShades[ThemeColor::Top];
    auto left = getTheme()->colorShades[ThemeColor::Left];
    auto right = getTheme()->colorShades[ThemeColor::Right];
    auto bottom = getTheme()->colorShades[ThemeColor::Bottom];

    switch (mVisual) {
        case Visual::FLAT:
            switch (mCorners) {
                case Corners::SQUARE:
                    context.fillRect(borderRect, mVisualMetrics.background);
                    break;
                case Corners::ROUND:
                default:
                    break;
            }
            break;
        case Visual::SHADOW:
            switch (mCorners) {
                case Corners::SQUARE: {
                    auto p0 = borderRect.point;
                    auto p3 = p0 + borderRect.size + -1;
                    Point p1{p3.x, p0.y};
                    Point p2{p0.x, p3.y};
                    for (ScreenCoordType idx = 0; idx < borderSize; ++idx) {
                        context.drawLine(p0, p1, (mActive ? bottom : top));
                        context.drawLine(p0, p2, (mActive ? right : left));
                        context.drawLine(p2, p3, (mActive ? top : bottom));
                        context.drawLine(p1, p3, (mActive ? left : right));
                        ++p0.x; ++p0.y; --p3.x; --p3.y; ++p2.x; --p2.y; --p1.x; ++p1.y;
                    }
                }
                case Corners::ROUND:
                default:
                    break;
            }
            break;
        case Visual::NOTCH:
        case Visual::RIDGE: {
            bool choice = mVisual == Visual::NOTCH;
            auto p0 = borderRect.point;
            auto p3 = p0 + borderRect.size + -1;
            Point p1{p3.x, p0.y};
            Point p2{p0.x, p3.y};
            for (ScreenCoordType idx = 0; idx < borderSize; ++idx) {
                auto flip = ((idx < borderSize / 2) == choice) | mActive;
                context.drawLine(p0, p1, (flip ? bottom : top));
                context.drawLine(p0, p2, (flip ? right : left));
                context.drawLine(p2, p3, (flip ? top : bottom));
                context.drawLine(p1, p3, (flip ? left : right));
                ++p0.x; ++p0.y; --p3.x; --p3.y; ++p2.x; --p2.y; --p1.x; ++p1.y;
            }
        }
            break;
        default:
            break;
    }
}

bool rose::Border::mouseButtonEvent(const SDL_MouseButtonEvent &e) {
    if (e.button == SDL_BUTTON_LMASK) {
        if (e.state == SDL_PRESSED && !mActive) {
            mActive = true;
            setNeedsDrawing();
        } else if (e.state == SDL_RELEASED && mActive) {
            mActive = false;
            setNeedsDrawing();
        }
        return true;
    }
    return Gadget::mouseButtonEvent(e);
}

bool rose::Border::enterLeaveEvent(bool enter, Uint32 ) {
    if (!enter && mActive) {
        mActive = false;
        setNeedsDrawing();
    }
    return true;
}
