//
// Created by richard on 25/01/23.
//

/*
 * Button.cpp Created by Richard Buckley (C) 25/01/23
 */

/**
 * @file Button.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 25/01/23
 */

#include "Button.h"

namespace rose {

    Button::Button(std::shared_ptr<Theme> &theme) : Border(theme) {
    }

    bool Button::mouseButtonEvent(const SDL_MouseButtonEvent &e) {
        if (e.button == SDL_BUTTON_LMASK) {
            if (e.state == SDL_PRESSED && !mActive) {
                setActive(true);
            } else if (e.state == SDL_RELEASED && mActive) {
                setActive(false);
                sendActivateSignal(SDL_GetTicks64());
            }
            return true;
        }
        return Border::mouseButtonEvent(e);
    }

    bool StateButton::mouseButtonEvent(const SDL_MouseButtonEvent &e) {
        if (e.button == SDL_BUTTON_LMASK) {
            if (e.state == SDL_PRESSED && !mActive) {
                setActive(true);
            } else if (e.state == SDL_RELEASED && mActive) {
                setActive(false);
                mButtonState = !mButtonState;
                sendStateChangeSignal(SDL_GetTicks64());
            }
            return true;
        }
        return Button::mouseButtonEvent(e);
    }

    bool Button::enterLeaveEvent(bool enter, Uint32 ) {
        if (!enter)
            setActive(false);
        return true;
    }

} // rose