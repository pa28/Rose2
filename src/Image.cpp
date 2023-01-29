//
// Created by richard on 29/01/23.
//

/*
 * Image.cpp Created by Richard Buckley (C) 29/01/23
 */

/**
 * @file Image.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 29/01/23
 */

#include "Image.h"
#include <Surface.h>

namespace rose {
    void Image::createTexture(Context &context) {
        if (!mImageFilePath.empty() && exists(mImageFilePath)) {
            Surface image{mImageFilePath};
            if (image) {
                image.textureFromSurface(context, mTexture);
                mVisualMetrics.desiredSize = mTexture.getSize();
            }
        }
    }

    void Image::draw(Context &context, Point drawLocation) {
        Gadget::draw(context, drawLocation);
        if (mTexture) {
            context.renderCopy(mTexture, mVisualMetrics.renderRect + drawLocation);
        }
    }

    bool Image::initialLayout(Context &context) {
        if (!mTexture) {
            try {
                createTexture(context);
                mVisualMetrics.desiredSize = mTexture.getSize();
            } catch (std::exception &e) {
                fmt::print("{}\n", e.what());
            }
        }
        return Gadget::initialLayout(context);
    }
} // rose