//
// Created by richard on 09/01/23.
//

/*
 * Border.h Created by Richard Buckley (C) 09/01/23
 */

/**
 * @file Border.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 09/01/23
 * @brief A Boarder places a frame around a single Gadget.
 * @details
 */

#ifndef ROSE2_BORDER_H
#define ROSE2_BORDER_H

#include "Rose.h"
#include "RoseTypes.h"
#include "GraphicsModel.h"
#include "manager/Singlet.h"

namespace rose {

    /**
     * @class Border
     * @details Border uses existing fields for special purposes:
     *      - gadgetPadding: to manage the border size, and assumes that all elements of gadgetPadding are the same value.
     *      - background: to draw a flat border.
     */
    class Border : public Singlet {
    protected:
        [[maybe_unused]] constexpr static std::string_view ClassName = "Border";

        Corners mCorners{};
        Visual mVisual{};
        bool mActive{false};

    public:
        Border() = default;
        explicit Border(std::shared_ptr<Theme>& theme);
        Border(const Border&) = delete;
        Border(Border&&) = default;
        Border& operator=(const Border&) = delete;
        Border& operator=(Border&&) = default;

        ~Border() override = default;

        const std::string_view& className() const override { return Border::ClassName; }

        bool initialLayout(Context &context) override;

        bool immediateGadgetLayout() override;

        void draw(Context &context, Point drawLocation) override;

        void setVisual(Visual visual) {
            mVisual = visual;
            setNeedsDrawing();
        }
    };

    /**
     * @brief Set the Visual parameter on a Boarder.
     * @tparam Arg The Visual parameter type.
     * @param gadget The Boarder gadget pointer.
     * @param arg The Visual value.
     */
    template<class Arg>
    requires std::is_same_v<Arg,Visual>
    void setParameter(std::shared_ptr<Border>& gadget, Arg arg) {
        gadget->setVisual(arg);
    }


} // rose


#endif //ROSE2_BORDER_H
