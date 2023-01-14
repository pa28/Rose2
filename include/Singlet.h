//
// Created by richard on 10/01/23.
//

/*
 * Singlet.h Created by Richard Buckley (C) 10/01/23
 */

/**
 * @file Singlet.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 10/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_SINGLET_H
#define ROSE2_SINGLET_H

#include "Gadget.h"

namespace rose {

    /**
     * @class Singlet
     */
    class Singlet : public Gadget {
        friend class LayoutManager;
        friend class Window;

    protected:
        constexpr static std::string_view ClassName = "Singlet";

        std::shared_ptr<Gadget> mGadget{};      ///< The single managed Gadget.

    public:
        Singlet() = default;
        Singlet(const Singlet&) = delete;
        Singlet(Singlet&&) = default;
        Singlet& operator = (const Singlet&) = delete;
        Singlet& operator = (Singlet&&) = default;

        bool initialLayout(Context &context) override;

        /**
         * @brief Add a Gadget to the management list of this Singlet.
         * @details This will include setting this Singlet as the manager of
         * the Gadget, which will remove the gadget from the management list
         * of its current manager, if any.
         * @param gadget The Gadget to manage.
         */
        virtual void manage(std::shared_ptr<Gadget> gadget);

        /**
         * @brief Add a Gadget from a builder to the management field of this Singlet
         * @param builder The Builder holding the new Gadget.
         */
        [[maybe_unused]] void manage(Builder &builder);

        /**
         * @brief Draw this Widget and all managed Gadgets.
         * @param context The graphics context to use.
         */
        void draw(Context &context, Point drawLocation) override;

    };

    /**
     * @class SingletBuilder.
     */
    class SingletBuilder : public GadgetBuilder {
    public:
        explicit SingletBuilder(std::shared_ptr<Gadget> g) : GadgetBuilder(std::move(g)) {}

        SingletBuilder() : GadgetBuilder(std::make_shared<Singlet>()) {}

        ~SingletBuilder() override = default;
    };

} // rose

#endif //ROSE2_SINGLET_H
