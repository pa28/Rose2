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
#include <Gadget.h>
#include <Window.h>
#include <GraphicsModel.h>

void rose::Border::createThemedBorder(rose::Context &, const rose::Theme &theme) {
    mCorners = theme.corners;
    mBackground = theme.gadgetBackground;
    mVisual = theme.visual;
}

void rose::Border::layout(rose::Context &context, Gadget &gadget) {
    auto visualMetrics = gadget.getVisualMetrics();
    if (visualMetrics.borderRect.size != getSize()) {
        Texture newTexture(context, visualMetrics.borderRect.size);
        reset(newTexture.get());
        mRendered = false;
    }
    if (!mRendered)
        renderBorder(context, gadget);
}

void rose::Border::draw(rose::Context &context, const rose::Point &drawPoint, Gadget &gadget) {
    if (this->operator bool()) {
        auto dest = gadget.getVisualMetrics().borderRect + drawPoint;
        context.renderCopy(*this, dest);
    }
}

void rose::Border::renderBorder(rose::Context &context, Gadget &gadget) {
    auto visualMetrics = gadget.getVisualMetrics();
    auto theme = gadget.getScreen()->getTheme();

    RenderTargetGuard targetGuard{context, *this};
    context.setDrawBlendMode(SDL_BLENDMODE_NONE);
    context.fillRect(Rectangle{Point{0,0}, this->getSize()}, theme.shades[Theme::UP_BG]);
    DrawColorGuard colorGuard{context, theme.shades[Theme::DN_BG]};
    auto borderSize = getSize();
    for (auto pixels = 0; pixels < visualMetrics.gadgetBorder; ++pixels) {
        Point p0{pixels, pixels}, p1{borderSize.w - pixels - 1, pixels};
        context.drawLine(p0, p1);
    }
    this->setBlendMode(SDL_BLENDMODE_BLEND);
    mRendered = true;
}
