//
// Created by richard on 04/01/23.
//

/*
 * RowColumn.h Created by Richard Buckley (C) 04/01/23
 */

/**
 * @file RowColumn.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 04/01/23
 * @brief Base class for layout managers.
 * @details
 */

#ifndef ROSE2_ROWCOLUMN_H
#define ROSE2_ROWCOLUMN_H

#include <Gadget.h>

namespace rose {

    class LinearLayout : public LayoutManager {
    public:
        enum class MajorAxis {
            HORIZONTAL, VERTICAL
        };

        enum class Alignment {
            TOP_LEFT, BOTTOM_RIGHT, CENTER
        };

        enum class AlignmentLoc {
            INNER, OUTER
        };

    protected:
        MajorAxis mMajorAxis{MajorAxis::HORIZONTAL};
        Alignment mAlignment{Alignment::TOP_LEFT};
        AlignmentLoc mAlignmentLoc{AlignmentLoc::INNER};

        ScreenCoordType mMajorAxisSize{};       ///< Total size of the major axis.
        ScreenCoordType mMinorAxisMax{};        ///< The minor axis maximum Gadget size.

    public:
        LinearLayout() = default;
        explicit LinearLayout(LinearLayout::MajorAxis majorAxis) : mMajorAxis(majorAxis) {}
        LinearLayout(const LinearLayout&) = delete;
        LinearLayout(LinearLayout&&) = default;
        LinearLayout& operator = (const LinearLayout&) = delete;
        LinearLayout& operator = (LinearLayout&&) = default;

        ~LinearLayout() override = default;

        void clearSizes() {
            mMajorAxisSize = mMinorAxisMax = 0;
        }

        /**
         * @brief Default layout strategy is to do nothing.
         * @param widget The widget to layout.
         * @return true on success, false on fail.
         */
        bool initialWidgetLayout(Context &context, std::shared_ptr<Widget> &widget) override;
    };

    /**
     * @class RowColumn
     */
    class RowColumn : public Widget {
    public:
        RowColumn() : Widget() { mLayoutManager = std::make_unique<LinearLayout>(); }
        RowColumn(const RowColumn&) = delete;
        RowColumn(RowColumn &&) = default;
        RowColumn& operator=(const RowColumn &) = delete;
        RowColumn& operator=(RowColumn&&) = default;

//        void draw(Context &context, Point drawLocation) override {
//            Widget::draw(context, drawLocation + getVisualMetrics().drawLocation);
//        }

        ~RowColumn() override = default;
    };

    /**
     * @class RowColumnBuilder
     */
    class RowColumnBuilder : public WidgetBuilder {
    public:
        [[maybe_unused]] explicit RowColumnBuilder(std::shared_ptr<Widget> w) : WidgetBuilder(std::move(w)) {}

        RowColumnBuilder() : WidgetBuilder(std::make_shared<RowColumn>()) {}

        ~RowColumnBuilder() override = default;
    };

    /**
     * @class Row
     */
    class Row : public RowColumn {
    public:
        Row() : RowColumn() { mLayoutManager = std::make_unique<LinearLayout>(); }
        Row(const Row&) = delete;
        Row(Row &&) = default;
        Row& operator=(const Row &) = delete;
        Row& operator=(Row&&) = default;

        ~Row() override = default;
    };

    /**
     * @class RowBuilder
     */
    class RowBuilder : public RowColumnBuilder {
    public:
        [[maybe_unused]] explicit RowBuilder(std::shared_ptr<Widget> w) : RowColumnBuilder(std::move(w)) {}

        RowBuilder() : RowColumnBuilder(std::make_shared<Row>()) {}

        ~RowBuilder() override = default;
    };

    /**
     * @class Column
     */
    class Column : public RowColumn {
    public:
        Column() : RowColumn() { mLayoutManager = std::make_unique<LinearLayout>(LinearLayout::MajorAxis::VERTICAL); }
        Column(const Column&) = delete;
        Column(Column &&) = default;
        Column& operator=(const Column &) = delete;
        Column& operator=(Column&&) = default;

        ~Column() override = default;
    };

    /**
     * @class ColumnBuilder
     */
    class ColumnBuilder : public RowColumnBuilder {
    public:
        [[maybe_unused]] explicit ColumnBuilder(std::shared_ptr<Widget> w) : RowColumnBuilder(std::move(w)) {}

        ColumnBuilder() : RowColumnBuilder(std::make_shared<Column>()) {}

        ~ColumnBuilder() override = default;
    };

} // rose

#endif //ROSE2_ROWCOLUMN_H
