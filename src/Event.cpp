//
// Created by richard on 06/01/23.
//

/*
 * Event.cpp Created by Richard Buckley (C) 06/01/23
 */

/**
 * @file Event.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 06/01/23
 */

#include "Event.h"
#include <fmt/format.h>

namespace rose {
    void Event::onEvent(SDL_Event &e) {
        switch (e.type) {
            case SDL_WINDOWEVENT:
                windowEvent(e.window);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                keyboardEvent(e.key);
                break;
            case SDL_TEXTINPUT:
                textInputEvent(e.text);
                break;
            case SDL_TEXTEDITING:
                throw EventException(fmt::format("Text editing event: {:x}", e.type));
            case SDL_MOUSEMOTION:
                mouseMotionEvent(e.motion);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouseButtonEvent(e.button);
                break;
            case SDL_MOUSEWHEEL:
                mouseWheelEvent(e.wheel);
                break;
            case SDL_FINGERMOTION:
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
                fingerTouchEvent(e.tfinger);
                break;
            case SDL_MULTIGESTURE:
            case SDL_KEYMAPCHANGED:
                break;
            default:
                throw EventException(fmt::format("Unhandled event: {:x}", e.type));
        }
    }

    void Event::windowEvent(SDL_WindowEvent &e) {
        switch (e.event) {
            case SDL_WINDOWEVENT_SHOWN:
                windowStateChange(Shown, e);
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                windowStateChange(Hidden, e);
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                windowStateChange(Exposed, e);
                break;
            case SDL_WINDOWEVENT_MOVED:
                windowPositionChange(Moved, Point{e.data1, e.data2});
                break;
            case SDL_WINDOWEVENT_RESIZED:
                windowSizeChange(Resized, e);
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                windowStateChange(SizeChanged, e);
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                windowStateChange(Minimized, e);
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                windowStateChange(Maximized, e);
                break;
            case SDL_WINDOWEVENT_RESTORED:
                windowStateChange(Restored, e);
                break;
            case SDL_WINDOWEVENT_ENTER:
                windowStateChange(Enter, e);
                break;
            case SDL_WINDOWEVENT_LEAVE:
                windowStateChange(Leave, e);
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                windowStateChange(Focus, e);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                windowStateChange(UnFocus, e);
                break;
            case SDL_WINDOWEVENT_CLOSE:
                windowStateChange(Close, e);
                break;
            case SDL_WINDOWEVENT_TAKE_FOCUS:
            case SDL_WINDOWEVENT_HIT_TEST:
                break;
        }
    }

    void Event::keyboardEvent(SDL_KeyboardEvent &e) {
        if (keyboardEventCallback)
            keyboardEventCallback(e);
    }

    void Event::mouseMotionEvent(SDL_MouseMotionEvent e) {
        if (mouseMotionEventCallback)
            mouseMotionEventCallback(e);
    }

    void Event::mouseButtonEvent(SDL_MouseButtonEvent &e) {
        if (mouseButtonEventCallback)
            mouseButtonEventCallback(e);
    }

    void Event::mouseWheelEvent(SDL_MouseWheelEvent &e) {
        if (mouseWheelEventCallback)
            mouseWheelEventCallback(e);
    }

    void Event::fingerTouchEvent(SDL_TouchFingerEvent &e) {
        if (fingerTouchEventCallback)
            fingerTouchEventCallback(e);
    }

    void Event::textInputEvent(SDL_TextInputEvent &e) {
        if (textInputEventCallback)
            textInputEventCallback(e);
    }
} // rose