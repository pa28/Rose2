//
// Created by richard on 04/01/23.
//

/*
 * Layout.h Created by Richard Buckley (C) 04/01/23
 */

/**
 * @file Layout.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 04/01/23
 * @brief Base class for layout managers.
 * @details
 */

#ifndef ROSE2_LAYOUT_H
#define ROSE2_LAYOUT_H

namespace rose {

    /**
     * @class Layout
     */
    class Layout {
    public:
        Layout() = default;
        Layout(const Layout&) = default;
        Layout(Layout &&) = default;
        Layout& operator=(const Layout &) = default;
        Layout& operator=(Layout&&) = default;


    };

} // rose

#endif //ROSE2_LAYOUT_H
