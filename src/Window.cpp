//
// Created by richard on 02/01/23.
//

/*
 * Window.cpp Created by Richard Buckley (C) 02/01/23
 */

#include "Window.h"

namespace rose {
    void Window::draw() {
        mGadgets.front()->draw(context());
    }

    [[maybe_unused]] void Window::setFocusGadget(std::shared_ptr<Gadget> &gadget) {
        clearFocusChain();
        while (gadget) {
            gadget->mHasFocus = true;
            mFocusChain.push_back(gadget);
            gadget = gadget->manager;
        }
    }

    void Window::clearFocusChain() {
        for (auto &gadget : mFocusChain) {
            gadget->mHasFocus = false;
        }

        mFocusChain.clear();
    }

    [[maybe_unused]] void Window::clearAllFocusFlags() {
        for (auto &gadget : mGadgets) {
            gadgetTraversal(gadget, [](std::shared_ptr<Gadget> &g) { g->mHasFocus = false; });
        }
    }

    void Window::gadgetTraversal(std::shared_ptr<Gadget> &topGadget,
                                 const std::function<void(std::shared_ptr<Gadget> &)> &lambda) {
        std::stack<std::shared_ptr<Gadget>> stack{};
        stack.push(topGadget);

        while (!stack.empty()) {
            auto gadget = stack.top();
            stack.pop();
            lambda(gadget);
            if (auto widget = std::dynamic_pointer_cast<Widget>(gadget); widget) {
                std::ranges::reverse_view reverseGadgetList{widget->mGadgetList};
                for (auto &newGadget : reverseGadgetList) {
                    stack.push(newGadget);
                }
            }
        }
    }
} // rose