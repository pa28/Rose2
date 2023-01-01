//
// Created by richard on 31/12/22.
//

/*
 * Gadget.h Created by Richard Buckley (C) 31/12/22
 */

#ifndef ROSE2_GADGET_H
#define ROSE2_GADGET_H

#include <string_view>
#include "Rose.h"

namespace rose {

/**
 * @class Gadget
 */
    class Gadget {
    public:
        constexpr static std::string_view ClassName = "Gadget";

    private:

    public:
        Gadget() = default;

        virtual ~Gadget() = default;

    };

} // rose

#endif //ROSE2_GADGET_H
