//
// Created by richard on 01/01/23.
//

/*
 * Manager.h Created by Richard Buckley (C) 01/01/23
 */

#ifndef ROSE2_MANAGER_H
#define ROSE2_MANAGER_H

#include <vector>
#include <memory>
#include <Gadget.h>

namespace rose {

/**
 * @class Manager
 */
    class Manager {
    protected:
        std::vector<std::unique_ptr<Gadget>> gadgetList{};

    public:
        Manager() = default;
        Manager(const Manager&) = delete;
        Manager(Manager&&) = default;
        Manager& operator = (const Manager &) = delete;
        Manager& operator = (Manager &&) = default;

        virtual ~Manager() = default;

    };

} // rose

#endif //ROSE2_MANAGER_H
