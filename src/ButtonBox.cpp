//
// Created by richard on 27/01/23.
//

/*
 * ButtonBox.cpp Created by Richard Buckley (C) 27/01/23
 */

/**
 * @file ButtonBox.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 27/01/23
 */

#include "ButtonBox.h"
#include "Button.h"

namespace rose {
    void ButtonBox::manage(std::shared_ptr<Gadget> gadget) {
        if (std::dynamic_pointer_cast<Button>(gadget))
            Widget::manage(gadget);
        throw SceneTreeError(fmt::format("ButtonBox can only manage objects derived from Button."));
    }

    void ButtonBox::manage(Builder &builder) {
        if (auto button = builder.get<Button>(); button)
            Widget::manage(builder);
        else
            throw SceneTreeError(fmt::format("ButtonBox can only manage objects derived from Button."));
    }
} // rose