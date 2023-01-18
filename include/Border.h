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

#include <Rose.h>
#include <RoseTypes.h>
#include <GraphicsModel.h>
#include <Singlet.h>

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
        Corners mCorners{Corners::UNSET};
        Visual mVisual{Visual::FLAT};
        bool mActive{false};

    public:
        Border() = default;
        Border(const Border&) = delete;
        Border(Border&&) = default;
        Border& operator=(const Border&) = delete;
        Border& operator=(Border&&) = default;

        ~Border() override = default;

        bool initialLayout(Context &context) override;

        bool forceInitialGadgetLayout() override;

        void draw(Context &context, Point drawLocation) override;
    };

    /**
     * @class SingletBuilder.
     */
    class BorderBuilder : public SingletBuilder {
    public:
        explicit BorderBuilder(std::shared_ptr<Gadget> g) : SingletBuilder(std::move(g)) {}

        BorderBuilder() : SingletBuilder(std::make_shared<Border>()) {}

        ~BorderBuilder() override = default;

        auto border(ScreenCoordType borderSize) {
            gadget->getVisualMetrics().gadgetPadding = borderSize;
            return *this;
        }
    };

} // rose


#endif //ROSE2_BORDER_H
