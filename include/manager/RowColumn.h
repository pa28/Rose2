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
 * @brief Base class for layout manager.
 * @details
 */

#ifndef ROSE2_ROWCOLUMN_H
#define ROSE2_ROWCOLUMN_H

#include "Widget.h"

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

        void setAlignment(Alignment alignment) { mAlignment = alignment; }

        void setMajorAxis(MajorAxis majorAxis) { mMajorAxis = majorAxis; }

        /**
         * @brief Default layout strategy is to do nothing.
         * @param gadget The widget to layout.
         * @return true on success, false on fail.
         */
        bool initialWidgetLayout(Context &context, std::shared_ptr<Gadget> &gadget) override;
    };

    /**
     * @class RowColumn
     */
    class RowColumn : public Widget {
    protected:
        constexpr static std::string_view ClassName = "RowColumn";

    public:
        RowColumn() : Widget() { mLayoutManager = std::make_unique<LinearLayout>(); }
        explicit RowColumn(std::shared_ptr<Theme>& theme) : Widget(theme) { mLayoutManager = std::make_unique<LinearLayout>(); }
        RowColumn(const RowColumn&) = delete;
        RowColumn(RowColumn &&) = default;
        RowColumn& operator=(const RowColumn &) = delete;
        RowColumn& operator=(RowColumn&&) = default;

        auto getLayoutManager() {
            return std::dynamic_pointer_cast<LinearLayout>(mLayoutManager);
        }

        ~RowColumn() override = default;

        const std::string_view& className() const override { return RowColumn::ClassName; }
    };

    /**
     * Set the LayoutManager major axis on a RowColumn.
     * @param gadget The pointer to the RowColumn.
     * @param arg The major axis value.
     */
    inline void setParameter(std::shared_ptr<RowColumn>& gadget, LinearLayout::MajorAxis arg) {
        gadget->getLayoutManager()->setMajorAxis(arg);
    }

    /**
     * @brief Set the LayoutManager alignment on a RowColumn.
     * @param gadget The pointer to the RowColumn.
     * @param arg The alignment value.
     */
    inline void setParameter(std::shared_ptr<RowColumn>& gadget, LinearLayout::Alignment arg) {
        gadget->getLayoutManager()->setAlignment(arg);
    }

    /**
     * @class Row
     */
    class Row : public RowColumn {
    protected:
        constexpr static std::string_view ClassName = "Row";

    public:
        Row() : RowColumn() { mLayoutManager = std::make_unique<LinearLayout>(); }
        explicit Row(std::shared_ptr<Theme>& theme) : RowColumn(theme)
            { mLayoutManager = std::make_unique<LinearLayout>(); }
        Row(const Row&) = delete;
        Row(Row &&) = default;
        Row& operator=(const Row &) = delete;
        Row& operator=(Row&&) = default;

        ~Row() override = default;

        const std::string_view& className() const override { return Row::ClassName; }
    };

    /**
     * @class Column
     */
    class Column : public RowColumn {
    protected:
        constexpr static std::string_view ClassName = "Column";

    public:
        Column() : RowColumn() { mLayoutManager = std::make_unique<LinearLayout>(LinearLayout::MajorAxis::VERTICAL); }
        explicit Column(std::shared_ptr<Theme>& theme) : RowColumn(theme)
            { mLayoutManager = std::make_unique<LinearLayout>(LinearLayout::MajorAxis::VERTICAL); }
        Column(const Column&) = delete;
        Column(Column &&) = default;
        Column& operator=(const Column &) = delete;
        Column& operator=(Column&&) = default;

        ~Column() override = default;

        const std::string_view& className() const override { return Column::ClassName; }
    };

} // rose

#endif //ROSE2_ROWCOLUMN_H
