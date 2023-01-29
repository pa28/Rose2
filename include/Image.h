/*
 * Image.h Created by Richard Buckley (C) 29/01/23
 */

/**
 * @file Image.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 29/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_IMAGE_H
#define ROSE2_IMAGE_H

#include <filesystem>
#include <utility>
#include "Gadget.h"

namespace rose {

/**
 * @class Image
 */
    class Image : public Gadget {
    protected:
        std::filesystem::path mImageFilePath{};
        Texture mTexture{};

    public:
        Image() = default;
        explicit Image(const std::shared_ptr<Theme>& theme) : Gadget(theme) {}
        Image(const Image &) = delete;
        Image(Image &&) = default;
        Image &operator=(const Image &) = delete;
        Image &operator=(Image &&) = default;
        ~Image() override = default;

        void createTexture(Context &context);

        void setFilePath(const std::filesystem::path &path) {
            mImageFilePath = path;
        }

        bool initialLayout(Context &context) override;

        void draw(Context &context, Point drawLocation) override;
    };

    inline void setParameter(std::shared_ptr<Image>& gadget, const std::filesystem::path& path) {
        gadget->setFilePath(path);
    }

} // rose

#endif //ROSE2_IMAGE_H
