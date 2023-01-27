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
#include "TextGadget.h"
#include "SceneTree.h"

namespace rose {

    Button::Button(std::shared_ptr<Theme> &theme) : Border(theme) {
    }

    bool Button::setActiveState(Uint8 state, Uint8 button, uint64_t ticks) {
        if (button == SDL_BUTTON_LMASK) {
            if (state == SDL_PRESSED && !mActive) {
                setActive(true);
            } else if (state == SDL_RELEASED && mActive) {
                setActive(false);
                sendActivateSignal(ticks);
            }
            return true;
        }
        return false;
    }

    bool Button::mouseButtonEvent(const SDL_MouseButtonEvent &e) {
        if (auto r = setActiveState(e.state, e.button, timestamp32to64(e.timestamp)); r) return r;
        return Border::mouseButtonEvent(e);
    }

    bool StateButton::setActiveState(Uint8 state, Uint8 button, uint64_t ticks) {
        if (button == SDL_BUTTON_LMASK) {
            if (state == SDL_PRESSED && !mActive) {
                setActive(true);
            } else if (state == SDL_RELEASED && mActive) {
                setActive(false);
                mButtonState = !mButtonState;
                setManagedIconCodePoint();
                sendStateChangeSignal(ticks);
            }
            return true;
        }
        return false;
    }

    StateButton::StateButton(std::shared_ptr<Theme> &theme) : Button(theme) {
        fmt::print("StateButton\n");
    }

    void StateButton::initialize() {
        setManagedIconCodePoint();
        Button::initialize();
    }

    void StateButton::setManagedIconCodePoint() {
        if (mIcon.expired()) {
            if (auto icon = findDesiredGadget<IconGadget>(shared_from_this()); icon) {
                mIcon = icon;
                icon->setIcon(mButtonState ? mOnCode : mOffCode);
            }
        } else {
            mIcon.lock()->setIcon(mButtonState ? mOnCode : mOffCode);
        }
    }

    bool StateButton::initialLayout(Context &context) {
        setManagedIconCodePoint();
        return Button::initialLayout(context);
    }

    bool Button::enterLeaveEvent(bool enter, Uint32 ) {
        if (!enter)
            setActive(false);
        return true;
    }

    bool MultiButton::setActiveState(Uint8 state, Uint8 button, uint64_t ticks) {
        if (state == SDL_PRESSED) {
            setActive(true);
        } else if (!mItems.empty()) {
            updateSignal.transmit(false, mItems.at(mActiveItem).itemId, ticks);
            if (button == SDL_BUTTON_LEFT) {
                mActiveItem = (mActiveItem + 1) % mItems.size();
            } else if (button == SDL_BUTTON_RIGHT) {
                mActiveItem = mActiveItem> 0 ? (mActiveItem - 1) % mItems.size() : mItems.size() - 1;
            }
            updateSignal.transmit(true, mItems.at(mActiveItem).itemId, ticks);
            setActive(false);
            setManagedIconCodePoint();
            return true;
        }
        return false;
    }
} // rose
