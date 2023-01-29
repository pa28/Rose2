/*
 * LabelButton.h Created by Richard Buckley (C) 29/01/23
 */

/**
 * @file PushButton.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 29/01/23
 * @brief Convenience classes for simple push buttons.
 * @details This file provides the following button types:
 *  - LabelButton: A push button with a text string face.
 *  - IconButton: A push button with an icon face.
 *  - BadgedButton: A push button with an icon badge and text string face.
 */

#ifndef ROSE2_PUSHBUTTON_H
#define ROSE2_PUSHBUTTON_H

#include <buttons/Button.h>
#include <manager/SceneTree.h>

namespace rose {

    /**
     * @class LabelButton
     * @brief A convenience class for creating a push button with a text string face.
     */
    class LabelButton : public Button {
    protected:
        constexpr static std::string_view ClassName = "LabelButton";     ///< class name

    public:
        LabelButton() = default;

        explicit LabelButton(std::shared_ptr<Theme> &theme) : Button(theme) {}

        LabelButton(const LabelButton &) = delete;

        LabelButton(LabelButton &&) = default;

        LabelButton &operator=(const LabelButton &) = delete;

        LabelButton &operator=(LabelButton &&) = default;

        ~LabelButton() override = default;

        const std::string_view& className() const override { return LabelButton::ClassName; }

        void completeCompositeConstruction(std::shared_ptr<Theme>& theme) override;

        template<class S>
        requires StringLike<S>
        void setText(S text) {
            if (auto textGadget = std::dynamic_pointer_cast<TextGadget>(mGadget); textGadget)
                textGadget->setText(text);
            else
                throw SceneTreeError("LabelButton is not managing a TextGadget.");
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
    [[maybe_unused]] void setEnumParameter(std::shared_ptr<LabelButton>& gadget, Parm parameter) {
        gadget->setText(parameter.data);
    }

    /**
     * @class IconButton
     * @brief A convenience class for creating a push button with an Icon face.
     */
    class IconButton : public Button {
    protected:
        constexpr static std::string_view ClassName = "IconButton";     ///< class name

    public:
        IconButton() = default;

        explicit IconButton(std::shared_ptr<Theme> &theme) : Button(theme) {}

        IconButton(const IconButton &) = delete;

        IconButton(IconButton &&) = default;

        IconButton &operator=(const IconButton &) = delete;

        IconButton &operator=(IconButton &&) = default;

        ~IconButton() override = default;

        const std::string_view& className() const override { return IconButton::ClassName; }

        void completeCompositeConstruction(std::shared_ptr<Theme>& theme) override;

        template<class S>
        requires StringLike<S>
        void setIcon(S iconCodePointName) {
            if (auto iconGadget = std::dynamic_pointer_cast<IconGadget>(mGadget); iconGadget)
                iconGadget->setIcon(iconCodePointName);
            else
                throw SceneTreeError("IconButton is not managing an IconGadget.");
        }
    };

    /**
     * @brief Set the icon used on the face of the button.
     * @tparam Parm The parameter type - param::Icon.
     * @param gadget Pointer to the IconButton.
     * @param parameter The new face icon.
     */
    template<class Parm>
    requires std::is_same_v<Parm,Parameter<MetaType::Icon,std::string>>
    [[maybe_unused]] void setEnumParameter(std::shared_ptr<IconButton>& gadget, Parm parameter) {
        gadget->setIcon(parameter.data);
    }

    /**
     * @class BadgedButton
     * @brief A convenience function for creating a push button with an Icon badge and text string face.
     */
    class BadgedButton : public Button {
    protected:
        constexpr static std::string_view ClassName = "BadgedButton";     ///< class name

    public:
        BadgedButton() = default;

        explicit BadgedButton(std::shared_ptr<Theme> &theme) : Button(theme) {}

        BadgedButton(const BadgedButton &) = delete;

        BadgedButton(BadgedButton &&) = default;

        BadgedButton &operator=(const BadgedButton &) = delete;

        BadgedButton &operator=(BadgedButton &&) = default;

        ~BadgedButton() override = default;

        const std::string_view& className() const override { return BadgedButton::ClassName; }

        void completeCompositeConstruction(std::shared_ptr<Theme>& theme) override;

        template<class S>
        requires StringLike<S>
        void setText(S text) {
            if (auto textGadget = findDesiredGadget<TextGadget>(shared_from_this()); textGadget)
                textGadget->setText(text);
            else
                throw SceneTreeError("BadgedButton is not managing an IconGadget.");
        }

        template<class S>
        requires StringLike<S>
        void setIcon(S iconCodePointName) {
            if (auto iconGadget = findDesiredGadget<IconGadget>(shared_from_this()); iconGadget)
                iconGadget->setIcon(iconCodePointName);
            else
                throw SceneTreeError("BadgedButton is not managing an IconGadget.");
        }
    };

    /**
     * @brief Set the text used on the face of the button.
     * @tparam Parm The parameter type - param::Text.
     * @param gadget Pointer to the BadgedButton.
     * @param parameter The new face text.
     */
    template<class Parm>
    requires std::is_same_v<Parm,Parameter<MetaType::Text,std::string>>
    [[maybe_unused]] void setEnumParameter(std::shared_ptr<BadgedButton>& gadget, Parm parameter) {
        gadget->setText(parameter.data);
    }

    /**
     * @brief Set the icon used on the face of the button.
     * @tparam Parm The parameter type - param::Icon.
     * @param gadget Pointer to the BadgedButton.
     * @param parameter The new face icon.
     */
    template<class Parm>
    requires std::is_same_v<Parm,Parameter<MetaType::Icon,std::string>>
    [[maybe_unused]] void setEnumParameter(std::shared_ptr<BadgedButton>& gadget, Parm parameter) {
        gadget->setIcon(parameter.data);
    }

} // rose

#endif //ROSE2_PUSHBUTTON_H
