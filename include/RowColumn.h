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

    protected:
        MajorAxis mMajorAxis{MajorAxis::HORIZONTAL};

    public:
        LinearLayout() = default;
        explicit LinearLayout(LinearLayout::MajorAxis majorAxis) : mMajorAxis(majorAxis) {}
        LinearLayout(const LinearLayout&) = delete;
        LinearLayout(LinearLayout&&) = default;
        LinearLayout& operator = (const LinearLayout&) = delete;
        LinearLayout& operator = (LinearLayout&&) = default;

        ~LinearLayout() override = default;

        /**
         * @brief Default layout strategy is to do nothing.
         * @param widget The widget to layout.
         * @return true on success, false on fail.
         */
         Point layoutWidget(Context &context, Rectangle constraint, std::shared_ptr<Widget> &widget) override;
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

        ~RowColumn() override = default;
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

} // rose

#endif //ROSE2_ROWCOLUMN_H
