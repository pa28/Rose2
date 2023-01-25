//
// Created by richard on 25/01/23.
//

/*
 * Button.h Created by Richard Buckley (C) 25/01/23
 */

/**
 * @file Button.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 25/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_BUTTON_H
#define ROSE2_BUTTON_H

#include <Border.h>

namespace rose {

    /**
     * @class Button
     * @brief Pure virtual base class for buttons.
     */
    class Button : public Border {
    protected:

    public:
        Button() = default;
        explicit Button(std::shared_ptr<Theme>& theme);
        Button(const Button&) = delete;
        Button(Button&&) = default;
        Button& operator=(const Button&) = delete;
        Button& operator=(Button&&) = default;

        ~Button() override = default;

        /**
         * @brief Signal to convey activation events such as button pressed.
         */
        ButtonStateProtocol::signal_type activateSignal{};

        void sendActivateSignal(uint64_t timeStamp) {
            activateSignal.transmit(true, timeStamp);
        }

        bool enterLeaveEvent(bool enter, Uint32 timestamp) override;

        bool mouseButtonEvent(const SDL_MouseButtonEvent &e) override;

//        void initialize() override = 0;

        virtual void setActive(bool active) {
            if (mActive != active) {
                mActive = active;
                setNeedsDrawing();
            }
        }
    };

    /**
     * @class ButtonBuilder.
     */
    class ButtonBuilder : public BorderBuilder {
    public:
        explicit ButtonBuilder(std::shared_ptr<Gadget> g) : BorderBuilder(std::move(g)) {}

        ButtonBuilder() : BorderBuilder(std::make_shared<Button>()) {}

        explicit ButtonBuilder(std::shared_ptr<Theme>& theme) : BorderBuilder(std::make_shared<Button>(theme)) {}

        ~ButtonBuilder() override = default;

    };

    class StateButton : public Button {
    protected:
        bool mButtonState{false};
        uint32_t mOffCode{}, mOnCode{};         // Icon code points for off and on state.

    public:
        StateButton() = default;
        explicit StateButton(std::shared_ptr<Theme>& theme);
        StateButton(const StateButton&) = delete;
        StateButton(StateButton&&) = default;
        StateButton& operator=(const StateButton&) = delete;
        StateButton& operator=(StateButton&&) = default;

        ~StateButton() override = default;

        /**
         * @brief Signal to convey the action of a stateful button.
         * @details The state field is true when the button changes to the active state, false when the button
         * changes to the inactive state.
         */
        ButtonStateProtocol::signal_type actionSignal{};

        bool mouseButtonEvent(const SDL_MouseButtonEvent &e) override;

        void sendStateChangeSignal(uint64_t timestamp) {
            actionSignal.transmit(mButtonState, timestamp);
        }

    };

    /**
     * @class StateButtonBuilder.
     */
    class StateButtonBuilder : public ButtonBuilder {
    public:
        explicit StateButtonBuilder(std::shared_ptr<Gadget> g) : ButtonBuilder(std::move(g)) {}

        StateButtonBuilder() : ButtonBuilder(std::make_shared<StateButton>()) {}

        explicit StateButtonBuilder(std::shared_ptr<Theme>& theme) : ButtonBuilder(std::make_shared<StateButton>(theme)) {}

        ~StateButtonBuilder() override = default;

    };

} // rose

#endif //ROSE2_BUTTON_H
