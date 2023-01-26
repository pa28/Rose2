/*
 * SceenTree.h Created by Richard Buckley (C) 25/01/23
 */

/**
 * @file SceenTree.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 25/01/23
 * @brief Free functions for manipulating the scene tree.
 * @details
 */

#ifndef ROSE2_SCENETREE_H
#define ROSE2_SCENETREE_H

#include <Gadget.h>
#include <functional>

namespace rose {

    class Singlet;
    class Widget;

    /**
     * @brief The working function behind findDesiredGadget
     * @param gadget The gadget to search from.
     * @param pred A unary predicate that tests for the fitness of a std::shared_ptr<Gadget>.
     * @param maxDepth The maximum depth to search.
     * @return The first found std::shared_ptr<Gadget> which passes the predicate.
     */
    std::shared_ptr<Gadget>
    searchTreeFor(std::shared_ptr<Gadget> &gadget, const std::function<bool(std::shared_ptr<Gadget> &)>& pred,
                  size_t maxDepth);

    /**
     * @brief A simple interface to find a Gadget, managed under this, of a specific type.
     * @tparam DesiredGadget The type of the desired Gadget to find.
     * @param gadget The gadget to start the search at.
     * @return The first found std::shared_ptr<Gadget> which can be down cast to DesiredGadget or a nullptr
     */
    template<class DesiredGadget>
    std::shared_ptr<DesiredGadget> findDesiredGadget(std::shared_ptr<Gadget> gadget) {
        auto found = searchTreeFor(gadget, [](std::shared_ptr<Gadget> g) -> bool {
            return std::dynamic_pointer_cast<DesiredGadget>(g).operator bool();
        }, 2);
        if (found)
            return std::dynamic_pointer_cast<DesiredGadget>(found);
        return nullptr;
    }


} // rose

#endif //ROSE2_SCENETREE_H
