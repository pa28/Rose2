//
// Created by richard on 20/01/23.
//

/*
 * TextSet.cpp Created by Richard Buckley (C) 20/01/23
 */

/**
 * @file TextSet.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 20/01/23
 */

#include "TextSet.h"

namespace rose {
    void TextSet::manage(std::shared_ptr<Gadget> gadget) {
        if (auto text = std::dynamic_pointer_cast<TextGadget>(gadget); text)
            Row::manage(gadget);
        else
            throw SceneTreeError(fmt::format("TextSet can not manage a {}", gadget->className()));
    }

    void TextSet::manage(Builder &builder) {
        if (auto text = builder.get<TextGadget>(); text)
            Row::manage(builder);
        else
            throw SceneTreeError(fmt::format("TextSet can not manage a {}", text->className()));
    }
} // rose