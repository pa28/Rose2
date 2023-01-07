//
// Created by richard on 06/01/23.
//

/*
 * Event.h Created by Richard Buckley (C) 06/01/23
 */

/**
 * @file Event.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 06/01/23
 * @brief Process, decode and distribute events from the SDL library.
 * @details
 */

#ifndef ROSE2_EVENT_H
#define ROSE2_EVENT_H

#include <SDL_events.h>
#include <stdexcept>
#include <functional>

#include <Rose.h>

namespace rose {

    /**
     * @enum WindowEventType
     * @brief The types of UI Window events.
     */
    enum WindowEventType : int {
        Shown, Hidden, Exposed, SizeChanged, Minimized, Maximized, FullScreen, Restored, Enter, Leave, Focus, UnFocus,
        Close, Moved, Resized,
    };

    /**
     * @class EventException
     * @brief Thrown on event handling errors.
     */
    class EventException : public std::runtime_error {
    public:
        explicit EventException(std::string &what_arg) : std::runtime_error(what_arg) {}
        explicit EventException(const std::basic_string<char>& what_arg) : std::runtime_error(what_arg) {}
    };

    /**
     * @class Event
     * @brief Decode and distribute events from the SDL library.
     */
    class Event {
    public:
        using WindowStateChangeCallback = std::function<void(WindowEventType, const SDL_WindowEvent&)>;
        using WindowPositionChangeCallback = std::function<void(WindowEventType, Point&)>;
        using WindowSizeChangeCallback = std::function<void(WindowEventType, Size&)>;
        using KeyboardEventCallback = std::function<bool(const SDL_KeyboardEvent&)>;
        using MouseMotionEventCallback = std::function<bool(const SDL_MouseMotionEvent&)>;
        using MouseButtonEventCallback = std::function<bool(const SDL_MouseButtonEvent&)>;
        using MouseWheelEventCallback = std::function<bool(const SDL_MouseWheelEvent&)>;
        using FingerTouchEventCallback = std::function<bool(const SDL_TouchFingerEvent&)>;
        using TextInputEventCallback = std::function<bool(const SDL_TextInputEvent&)>;

        void windowStateChange(WindowEventType type, const SDL_WindowEvent &event) {
            if (windowStateChangeCallback)
                windowStateChangeCallback(type, event);
        }

        void windowSizeChange(WindowEventType type, Size size) {
            if (windowSizeChangeCallback)
                windowSizeChangeCallback(type, size);
        }

        void windowPositionChange(WindowEventType type, Point position) {
            if (windowPositionChangeCallback)
                windowPositionChangeCallback(type, position);
        }

    protected:
        /// The Application the EventSemantics are processed for.
        WindowSizeChangeCallback windowSizeChangeCallback;
        WindowPositionChangeCallback windowPositionChangeCallback;
        std::function<void(WindowEventType, const SDL_WindowEvent&)> windowStateChangeCallback;
        KeyboardEventCallback keyboardEventCallback;
        std::function<bool(const SDL_MouseMotionEvent&)> mouseMotionEventCallback;
        std::function<bool(const SDL_MouseButtonEvent&)> mouseButtonEventCallback;
        MouseWheelEventCallback mouseWheelEventCallback;
        FingerTouchEventCallback fingerTouchEventCallback;
        TextInputEventCallback textInputEventCallback;

    public:
        Event() = default;
        Event(const Event&) = delete;
        Event(Event&&) = default;
        Event& operator=(const Event &) = delete;
        Event& operator=(Event &&) = default;

        ~Event() = default;

        void onEvent(SDL_Event &e);

        void windowEvent(SDL_WindowEvent &e);

        void keyboardEvent(SDL_KeyboardEvent &e);

        void mouseMotionEvent(SDL_MouseMotionEvent e);

        void mouseButtonEvent(SDL_MouseButtonEvent &e);

        void mouseWheelEvent(SDL_MouseWheelEvent &e);

        void fingerTouchEvent(SDL_TouchFingerEvent &e);

        void textInputEvent(SDL_TextInputEvent &e);

        void setWinSizeChange(WindowSizeChangeCallback &cb) {
            windowSizeChangeCallback = cb;
        }

        void setWinMove(WindowPositionChangeCallback &cb) {
            windowPositionChangeCallback = cb;
        }

        void setWinStateChange(std::function<void(WindowEventType, const SDL_WindowEvent&)>&& cb) {
            windowStateChangeCallback = std::move(cb);
        }

        void setKeEvent(KeyboardEventCallback &cb) {
            keyboardEventCallback = cb;
        }

        void setMouseMotion(std::function<bool(const SDL_MouseMotionEvent&)>&& cb) {
            mouseMotionEventCallback = std::move(cb);
        }

        void setMouseButton(std::function<bool(const SDL_MouseButtonEvent&)>&& cb) {
            mouseButtonEventCallback = std::move(cb);
        }

        void setMouseWheel(MouseWheelEventCallback &cb) {
            mouseWheelEventCallback = cb;
        }

        void setFingerTouch(FingerTouchEventCallback &cb) {
            fingerTouchEventCallback = cb;
        }

        void setTextInput(TextInputEventCallback &cb) {
            textInputEventCallback = cb;
        }
    };

} // rose

#endif //ROSE2_EVENT_H
