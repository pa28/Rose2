//
// Created by richard on 27/01/23.
//

/*
 * ButtonBox.h Created by Richard Buckley (C) 27/01/23
 */

/**
 * @file ButtonBox.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 27/01/23
 * @brief Base class for containers of stateful buttons.
 * @details Forms the basis for RadioBox and SwitchBox.
 */

#ifndef ROSE2_BUTTONBOX_H
#define ROSE2_BUTTONBOX_H

#include <utility>

#include "manager/RowColumn.h"

namespace rose {

/**
 * @class ButtonBox
 */
    class ButtonBox : public RowColumn {

    public:
        ButtonBox() : RowColumn() { mLayoutManager = std::make_unique<LinearLayout>(); }

        explicit ButtonBox(std::shared_ptr<Theme> &theme) : RowColumn(
                theme) { mLayoutManager = std::make_unique<LinearLayout>(); }

        ButtonBox(const ButtonBox &) = delete;

        ButtonBox(ButtonBox &&) = default;

        ButtonBox &operator=(const ButtonBox &) = delete;

        ButtonBox &operator=(ButtonBox &&) = default;

        ~ButtonBox() override = default;

        /**
         * @brief Overridden to restrict managed Gadgets to Buttons.
         * @param gadget the Gadget to manage.
         */
        [[maybe_unused]] void manage(std::shared_ptr<Gadget> gadget) override;
    };

    /**
     * Set the LayoutManager major axis on a RowColumn.
     * @param gadget The pointer to the ButtonBox.
     * @param arg The major axis value.
     */
    inline void setParameter(std::shared_ptr<ButtonBox>& gadget, LinearLayout::MajorAxis arg) {
        gadget->getLayoutManager()->setMajorAxis(arg);
    }

    /**
     * @brief Set the LayoutManager alignment on a RowColumn.
     * @param gadget The pointer to the ButtonBox.
     * @param arg The alignment value.
     */
    inline void setParameter(std::shared_ptr<ButtonBox>& gadget, LinearLayout::Alignment arg) {
        gadget->getLayoutManager()->setAlignment(arg);
    }


} // rose

#endif //ROSE2_BUTTONBOX_H
