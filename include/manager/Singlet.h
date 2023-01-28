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

        [[maybe_unused]] void debugLayout(const char *func);

    public:
        Singlet() = default;
        explicit Singlet(std::shared_ptr<Theme>& theme) : Gadget(theme) {}
        Singlet(const Singlet&) = delete;
        Singlet(Singlet&&) = default;
        Singlet& operator = (const Singlet&) = delete;
        Singlet& operator = (Singlet&&) = default;

        std::shared_ptr<Gadget> getManagedGadget() { return mGadget; }

        void initialize() override;

        const std::string_view& className() const override { return Singlet::ClassName; }

        bool initialLayout(Context &context) override;

        /**
         * @brief Set internal alignment padding.
         * @details Internal alignment padding is used to align Gadgets inside of manager. Internal alignment
         * may be set directly by using the getVisualMetrics() accessor function; however this method should
         * be used as it allows manager to place the padding in an appropriate location.
         * @param padding the padding.
         */
        [[maybe_unused]] void setInternalAlignmentPadding(const Padding& padding) override;

        /**
         * @brief Immediate Gadget layout calculation.
         * @details Compute the layout of the Gadget locally using only what is know to the Gadget.
         * @return
         */
        bool immediateGadgetLayout() override;

        /**
         * @brief Add a Gadget to the management list of this Singlet.
         * @details This will include setting this Singlet as the manager of
         * the Gadget, which will remove the gadget from the management list
         * of its current manager, if any.
         * @param gadget The Gadget to manage.
         */
        [[maybe_unused]] virtual void manage(std::shared_ptr<Gadget> gadget);

        /**
         * @brief Draw this Widget and all managed Gadgets.
         * @param context The graphics context to use.
         */
        void draw(Context &context, Point drawLocation) override;

        void expose(Context &context, Rectangle exposed) override;

    };

} // rose

#endif //ROSE2_SINGLET_H
