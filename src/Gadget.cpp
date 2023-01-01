//
// Created by richard on 31/12/22.
//

/*
 * Gadget.cpp Created by Richard Buckley (C) 31/12/22
 */

#include "Gadget.h"

namespace rose {
    void Gadget::managedBy(std::shared_ptr<Widget> widget) {
        manager = std::move(widget);
    }

    void Widget::manage(std::shared_ptr<Gadget> gadget) {
        if (auto manager = gadget->getManager(); manager) {
            manager->unManage(gadget);
        }
        gadget->managedBy(shared_from_this());
        gadgetList.push_back(std::move(gadget));
    }

    void Widget::unManage(const std::shared_ptr<Gadget> &gadget) {
        gadgetList.erase(std::remove(gadgetList.begin(), gadgetList.end(), gadget), gadgetList.end());
    }
} // rose