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
        [[maybe_unused]] constexpr static std::string_view ClassName = "Singlet";

        std::shared_ptr<Gadget> mGadget{};      ///< The single managed Gadget.

        [[maybe_unused]] void debugLayout(const char *func);

    public:
        Singlet() = default;
        Singlet(const Singlet&) = delete;
        Singlet(Singlet&&) = default;
        Singlet& operator = (const Singlet&) = delete;
        Singlet& operator = (Singlet&&) = default;

        bool initialLayout(Context &context) override;

        /**
         * @brief Set internal alignment padding.
         * @details Internal alignment padding is used to align Gadgets inside of managers. Internal alignment
         * may be set directly by using the getVisualMetrics() accessor function; however this method should
         * be used as it allows managers to place the padding in an appropriate location.
         * @param padding the padding.
         */
        [[maybe_unused]] void setInternalAlignmentPadding(const Padding& padding) override;

        /**
         * @brief Force initial gadget layout avoiding virtual method.
         * @details This may be required for Widgets to call after they have configured their Gadgets.
         * @return
         */
        bool forceInitialGadgetLayout() override;

        /**
         * @brief Add a Gadget to the management list of this Singlet.
         * @details This will include setting this Singlet as the manager of
         * the Gadget, which will remove the gadget from the management list
         * of its current manager, if any.
         * @param gadget The Gadget to manage.
         */
        [[maybe_unused]] virtual void manage(std::shared_ptr<Gadget> gadget);

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
