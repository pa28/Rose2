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
 * The there are three supported ways of providing a face:
 *  - Attaching a TextGadget.
 *  - Attaching an IconGadget or other derivative of TextGadget.
 *  - Attaching a TextSet which may contain multiple TextGadgets or IconGadgets.
 * <br/>
 * More complex buttons construct their managed subtree based on a specification provided. These are easier to use
 * but less flexible.
 */

#ifndef ROSE2_BUTTON_H
#define ROSE2_BUTTON_H

#include "manager/Border.h"
#include "TextGadget.h"
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

        explicit Button(std::shared_ptr<Theme> &theme);

        Button(const Button &) = delete;

        Button(Button &&) = default;

        Button &operator=(const Button &) = delete;

        Button &operator=(Button &&) = default;

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
     * @class StateButton
     * @brief A button which has a boolean state indicated by one of two icons.
     * @details This class is the base for Toggle, Check, and Radio buttons. The state of the button is indicated
     * by displaying one of two specified icons. The StateButton manages a single Gadget which provides the button
     * face which must include an IconGadget nested no deeper than two levels down. If multiple IconGadgets are
     * subordinate to the StateButton within reach, the first encountered is used.
     */
    class StateButton : public Button {
    public:
        struct Icons {
            std::string off, on;
        };

        enum Type {
            Radio, Check, Toggle
        };

    protected:
        bool mButtonState{false};
        uint32_t mOffCode{}, mOnCode{};         // Icon code points for off and on state.
        std::weak_ptr<IconGadget> mIcon{};      // Local pointer to the managed IconGadget.
        std::weak_ptr<TextGadget> mText{};      // Local pointer to the managed TextGadget.

    public:
        StateButton() = default;

        explicit StateButton(std::shared_ptr<Theme> &theme);

        StateButton(const StateButton &) = delete;

        StateButton(StateButton &&) = default;

        StateButton &operator=(const StateButton &) = delete;

        StateButton &operator=(StateButton &&) = default;

        ~StateButton() override = default;

        void completeCompositeConstruction(std::shared_ptr<Theme>& ) override;

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

        template<class S>
        requires StringLike<S>
        void setText(S text) {
            setManagedTextString(std::string(text));
        }

        /**
         * @brief Find the managed IconGadget and change its code point to match the state of the button.
         */
        void setManagedIconCodePoint();

        /**
         * @brief Find the managed TextGadget and change its text value to the provided std::string value.
         * @param text
         */
        void setManagedTextString(const std::string& text);

        void initialize() override;

        void radioButton() {
            setIcons("radio_button_unchecked", "radio_button_checked");
        }

        void checkBox() {
            setIcons("check_box_outline_blank", "check_box");
        }

        void toggleButton() {
            setIcons("toggle_off", "toggle_on");
        }
    };

    /**
     * @brief Set the text used on the face of the button.
     * @tparam Parm The parameter type - param::Text.
     * @param gadget Pointer to the StateButton.
     * @param parameter The new face text.
     */
    template<class Parm>
    requires std::is_same_v<Parm,Parameter<MetaType::Text,std::string>>
    [[maybe_unused]] void setEnumParameter(std::shared_ptr<StateButton>& gadget, Parm parameter) {
        gadget->setText(parameter.data);
    }

    /**
     * @brief Set the Icons used to indicate button state.
     * @tparam Parm The parameter - a rose::StateButton::Icons type.
     * @param gadget The StateButton pointer.
     * @param parameter The rose::StateButton::Icons value.
     */
    template<class Parm>
    requires std::is_same_v<Parm, StateButton::Icons>
    [[maybe_unused]] void setParameter(std::shared_ptr<StateButton>& gadget, Parm parameter) {
        gadget->setIcons(parameter.off, parameter.on);
    }

    /**
     * @brief Set the Icons used to indicate button state.
     * @tparam Parm The parameter - a Rose::StateButton::Type type.
     * @param gadget The StateButton pointer.
     * @param parameter The rose::StateButton::Type value.
     */
    template<class Parm>
    requires std::is_same_v<Parm, StateButton::Type>
    [[maybe_unused]] void setParameter(std::shared_ptr<StateButton>& gadget, Parm parameter) {
        switch (parameter) {
            case StateButton::Radio:
                gadget->radioButton();
                break;
            case StateButton::Check:
                gadget->checkBox();
                break;
            case StateButton::Toggle:
                gadget->toggleButton();
                break;
        }
    }

    /**
     * @class MultiButton
     * @brief A compact Icon only multiple state button.
     * @details The MultiButton provides control similar to the familiar Radio button paradigm but in a much more
     * compact form. The face of the button is provided by a single IconGadget which is created and attached when
     * the MultiButton is built. The user provides a set of MultiButton::Item values which determine the number of
     * states and the Icon displayed for each state.
     * <br/>
     * The button moves forward through the list of states with a left mouse button click, backward with a right
     * mouse click.
     * @throws SceneTreeError if the managed Gadget is not an IconGadget.
     */

    template<class Range>
    concept MultiButtonItemRange = requires(Range &range) {
        std::ranges::begin(range);
        std::ranges::end(range);
        { std::ranges::begin(range) } -> std::convertible_to<std::tuple<uint32_t, std::string_view> *>;
    };

    class MultiButton : public Button {
    public:
        struct Item {
            uint32_t itemId{}, codePoint{};
        };

        using TextItem = std::tuple<uint32_t, std::string_view>;

        template<size_t size>
        using ItemArray = std::array<TextItem,size>;

        using ItemListType = std::vector<Item>;

        using IteratorType = ItemListType::iterator;

    protected:
        ItemListType mItems{};
        ItemListType::size_type mActiveItem{};

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

        explicit MultiButton(std::shared_ptr<Theme> &theme) : Button(theme) {}

        MultiButton(const MultiButton &) = delete;

        MultiButton(MultiButton &&) = default;

        MultiButton &operator=(const MultiButton &) = delete;

        MultiButton &operator=(MultiButton &&) = default;

        ~MultiButton() override = default;

        void completeCompositeConstruction(std::shared_ptr<Theme>& theme) override {
            if (auto icon = Build<IconGadget>(theme); icon) {
                manage(icon);
                return;
            }
            throw SceneTreeError("Can not complete build of MultiButton by adding IconGadget, IconGadget not built.");
        }

        /**
         * @brief Get an iterator pointing to the first state item.
         * @return IteratorType
         */
        auto begin() { return mItems.begin(); }

        /**
         * @brief Get an iterator pointing one past the last state item.
         * @return IteratorType
         */
        auto end() { return mItems.end(); }

        template<class Range>
        requires MultiButtonItemRange<Range>
        void setItems(const Range &buttonItems) {
            mItems.clear();
            addItems(buttonItems);
        }

        /**
         * @brief Add the provided range of items to the button item list.
         * @details The range must be of values convertable to std::tuple<uint32_t,std::string_view>*
         * @tparam Range the type of the range.
         * @param items the range of items.
         */
        template<class Range>
        requires MultiButtonItemRange<Range>
        void addItems(const Range &items) {
            for (const auto &item: items) {
                const auto [itemNumber, codePointName] = item;
                mItems.emplace_back(itemNumber, IconGadget::getIcon(codePointName));
            }
            setManagedIconCodePoint();
        }

        /**
         * @brief Use decode mouse button data to set the active state of the button.
         * @param state The mouse button state.
         * @param button The button that changed.
         * @param ticks When the change happened.
         * @return true if the event was consumed.
         */
        bool setActiveState(Uint8 state, Uint8 button, uint64_t ticks) override;

        /**
         * @brief The update signal transmitted when the button state is changed.
         * @details The lambda is called when a new connection is made to the signal sending the current state
         * data to the new subscriber.
         */
        MultiButtonProtocol::signal_type updateSignal{[this]() {
            for (auto &item: mItems) {
                updateSignal.transmitLastConnected(item.itemId == mActiveItem, item.itemId, SDL_GetTicks64());
            }
        }};
    };

    template<class GadgetType, class Parm>
    requires MultiButtonItemRange<Parm> && std::derived_from<GadgetType, MultiButton>
    void setParameter(std::shared_ptr<GadgetType>& gadget, Parm parameter) {
        gadget->setItems(parameter);
    }

} // rose

#endif //ROSE2_BUTTON_H
