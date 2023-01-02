//
// Created by richard on 02/01/23.
//

/*
 * Window.cpp Created by Richard Buckley (C) 02/01/23
 */

#include "Window.h"

namespace rose {
    void Window::draw() {
        mWidgets.front()->draw(context());

    }
} // rose