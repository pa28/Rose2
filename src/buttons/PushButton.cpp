//
// Created by richard on 29/01/23.
//

/*
 * LabelButton.cpp Created by Richard Buckley (C) 29/01/23
 */

/**
 * @file LabelButton.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 29/01/23
 */

#include "buttons/PushButton.h"
#include "manager/TextSet.h"

namespace rose {
    void LabelButton::completeCompositeConstruction(std::shared_ptr<Theme> &theme) {
        if (auto text = Build<TextGadget>(theme); text)
            manage(text);
    }

    void IconButton::completeCompositeConstruction(std::shared_ptr<Theme> &theme) {
        if (auto icon = Build<IconGadget>(theme))
            manage(icon);
    }

    void BadgedButton::completeCompositeConstruction(std::shared_ptr<Theme> &theme) {
        if (auto set = Build<TextSet>(theme); set) {
            if (auto icon = Build<IconGadget>(theme))
                set->manage(icon);
            if (auto text = Build<TextGadget>(theme); text)
                set->manage(text);
            manage(set);
        }
    }
} // rose