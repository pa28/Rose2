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
 * @brief Foundation class for, and some simple Buttons.
 * @details Buttons are constructed as an animatable Border (which is derived from Singlet) which surrounds the
 * image of the button face. In some case the user program is responsible for constructing the scene subtree, and
 * attaching it, which makes up the face of the button. This could be almost anything that is possible to construct
 * with the library, however elements which consume events required for the correct operation of a button will,
 * of course, cause erratic and possible unusable program behavior.
 * <br/>
 * More complex buttons construct their managed subtree based on a specification provided. These are easier to use
 * but less flexible.
 */

#ifndef ROSE2_BUTTON_H
#define ROSE2_BUTTON_H

#include <Border.h>
#include <TextGadget.h>
#include <ranges>

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

        virtual bool setActiveState(Uint8 state, Uint8 button, uint64_t ticks);

        /**
         * @brief Signal to convey activation events such as button pressed.
         */
        ButtonStateProtocol::signal_type activateSignal{};

        /**
         * @brief Transmit the activate signal to attached slots.
         * @param timeStamp The 64 bit timestamp at the time of sending.
         */
        void sendActivateSignal(uint64_t timeStamp) {
            activateSignal.transmit(true, timeStamp);
        }

        /**
         * @brief Process mouse enter and leave events.
         * @param enter true when the mouse enters, false when it leaves.
         * @param timestamp The SDL 32 bit time stamp from SDL_Event.
         * @return
         */
        bool enterLeaveEvent(bool enter, Uint32 timestamp) override;

        /**
         * @brief Process mouse button events.
         * @param e the mouse button event structure.
         * @return true if the event was consumed.
         */
        bool mouseButtonEvent(const SDL_MouseButtonEvent &e) override;

        /**
         * @brief Set the button active state.
         * @details Iff the button state changes it requests redrawing.
         * <br/>Visual semantics can differ but the active state is intended to indicate to the user the button
         * is currently 'pressed'.
         * @param active the (possibly) new button state.
         */
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

        bool setActiveState(Uint8 state, Uint8 button, uint64_t ticks) override;

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

    template<class Range>
    concept MultiButtonItemRange = requires(Range& range) {
        std::ranges::begin(range);
        std::ranges::end(range);
        { std::ranges::begin(range) } -> std::convertible_to<std::tuple<uint32_t,std::string_view>*>;
    };

    class MultiButton : public Button {
    public:
        struct  Item {
            uint32_t itemId{}, codePoint{};
        };

        using ItemListType = std::vector<Item>;

        using IteratorType = ItemListType::iterator;

    protected:
        ItemListType mItems{};
        ItemListType ::size_type mActiveItem{};

        /**
         * @brief Set the icon code point on the managed IconGadget.
         * @throws SceneTreeError if the button does not manage an IconGadget.
         */
        void setManagedIconCodePoint() {
            if (auto icon = std::dynamic_pointer_cast<IconGadget>(mGadget); icon) {
                icon->setIcon(mItems.at(mActiveItem).codePoint);
            } else {
                throw SceneTreeError(fmt::format("MultiButton does not manage an IconGadget {} {}",
                                                 __FILE__, __LINE__));
            }
        }

    public:
        MultiButton() = default;
        explicit MultiButton(std::shared_ptr<Theme>& theme) : Button(theme) {}
        MultiButton(const MultiButton&) = delete;
        MultiButton(MultiButton&&) = default;
        MultiButton& operator=(const MultiButton&) = delete;
        MultiButton& operator=(MultiButton&&) = default;

        ~MultiButton() override = default;

        auto begin() { return mItems.begin(); }

        auto end() { return mItems.end(); }

        template<class Range>
                requires MultiButtonItemRange<Range>
        void addItems(const Range& items) {
            for (const auto& item : items ) {
                const auto [itemNumber, codePointName] = item;
                mItems.emplace_back(itemNumber, IconGadget::getIcon(codePointName));
            }
            setManagedIconCodePoint();
        }

        bool setActiveState(Uint8 state, Uint8 button, uint64_t ticks) override;

        MultiButtonProtocol::signal_type updateSignal{[this]() {
            for (auto & item : mItems) {
                updateSignal.transmitLastConnected(item.itemId == mActiveItem, item.itemId, SDL_GetTicks64());
            }
        }};
    };

    class MultiButtonBuilder : public ButtonBuilder {
    public:
        explicit MultiButtonBuilder(std::shared_ptr<Gadget> g) : ButtonBuilder(std::move(g)) {}

        MultiButtonBuilder() : ButtonBuilder(std::make_shared<MultiButton>()) {
            if (auto icon = IconGadgetBuilder{}; icon)
                icon >> *this;
        }

        explicit MultiButtonBuilder(std::shared_ptr<Theme>& theme) : ButtonBuilder(std::make_shared<MultiButton>(theme)) {
            if (auto icon = IconGadgetBuilder{theme}; icon)
                icon >> *this;
        }

        ~MultiButtonBuilder() override = default;

        template<class Range>
                requires MultiButtonItemRange<Range>
        auto items(const Range& buttonItems) {
            std::dynamic_pointer_cast<MultiButton>(gadget)->addItems(buttonItems);
            return *this;
        }
    };

} // rose

#endif //ROSE2_BUTTON_H
