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
#include <TextGadget.h>

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
        std::weak_ptr<IconGadget> mIcon{};      // Local pointer to the managed IconGadget.

    public:
        StateButton() = default;
        explicit StateButton(std::shared_ptr<Theme>& theme);
        StateButton(const StateButton&) = delete;
        StateButton(StateButton&&) = default;
        StateButton& operator=(const StateButton&) = delete;
        StateButton& operator=(StateButton&&) = default;

        ~StateButton() override = default;

        bool initialLayout(Context &context) override;

        void draw(Context &context, Point drawLocation) override {
            Button::draw(context, drawLocation);
        }

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

        /**
         * @brief Set the off and on icon code points.
         * @param off The off code point.
         * @param on The on code point.
         */
        void setIcons(uint32_t off, uint32_t on) {
            mOffCode = off;
            mOnCode = on;
        }

        /**
         * @brief Set the off and on icon code points from names.
         * @throws CodePointError
         * @tparam S1 Type of off name
         * @tparam S2 Type of on name
         * @param off Code point name for the off icon.
         * @param on Code point name for the on icon.
         */
        template<class S1, class S2>
                requires StringLike<S1> && StringLike<S2>
        void setIcons(S1 off, S2 on) {
            auto cpOff = IconGadget::getIcon(off);
            auto cpOn = IconGadget::getIcon(on);
            setIcons(cpOff, cpOn);
        }

        void setManagedIconCodePoint();

        void initialize() override;
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

        auto setIcons(uint32_t off, uint32_t on) {
            std::dynamic_pointer_cast<StateButton>(gadget)->setIcons(off, on);
            return *this;
        }

        template<class S1, class S2>
                requires StringLike<S1> && StringLike<S2>
        auto setIcons(S1 off, S2 on) {
            std::dynamic_pointer_cast<StateButton>(gadget)->setIcons(off, on);
            return *this;
        }

        auto toggleButton() {
            std::dynamic_pointer_cast<StateButton>(gadget)->setIcons("toggle_off", "toggle_on");
            return *this;
        }

        auto checkBox() {
            std::dynamic_pointer_cast<StateButton>(gadget)->setIcons("check_box_outline_blank", "check_box");
            return *this;
        }

        auto radioButton() {
            std::dynamic_pointer_cast<StateButton>(gadget)->setIcons("radio_button_unchecked", "radio_button_checked");
            return *this;
        }
    };

} // rose

#endif //ROSE2_BUTTON_H
