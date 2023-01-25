//
// Created by richard on 25/01/23.
//

/*
 * SceenTree.cpp Created by Richard Buckley (C) 25/01/23
 */

/**
 * @file SceenTree.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 25/01/23
 */

#include "SceneTree.h"
#include <Rose.h>
#include <RoseTypes.h>
#include <Singlet.h>
#include <Widget.h>
#include <stack>

namespace rose {
    std::shared_ptr<Gadget>
    searchTreeFor(std::shared_ptr<Gadget> &gadget, const std::function<bool(std::shared_ptr<Gadget> &)>& pred,
                  size_t maxDepth) {
        std::stack<std::shared_ptr<Gadget>> stack{};
        size_t depth{0};

        stack.push(gadget);
        while (!stack.empty()) {
            gadget = stack.top();
            stack.pop();
            if (pred(gadget))
                return gadget;

            if (depth < maxDepth) {
                if (auto singlet = std::dynamic_pointer_cast<Singlet>(gadget); singlet) {
                    ++depth;
                    stack.push(singlet->getManagedGadget());
                } else if (auto widget = std::dynamic_pointer_cast<Widget>(gadget); widget) {
                    ++depth;
                    for (auto &g: (*widget)) {
                        stack.push(g);
                    }
                }
            }
        }

        return nullptr;
    }
} // rose