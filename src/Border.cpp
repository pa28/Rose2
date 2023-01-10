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

void rose::Border::createThemedBorder(rose::Context &, const rose::Theme &theme) {
    mCorners = theme.corners;
    mBackground = theme.gadgetBackground;
    mVisual = theme.visual;
}

void rose::Border::layout(rose::Context &) {

}

void rose::Border::draw(rose::Context &, const rose::Point &) {

}
