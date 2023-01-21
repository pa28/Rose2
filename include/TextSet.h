//
// Created by richard on 20/01/23.
//

/*
 * TextSet.h Created by Richard Buckley (C) 20/01/23
 */

/**
 * @file TextSet.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 20/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_TEXTSET_H
#define ROSE2_TEXTSET_H

#include <Rose.h>
#include <RoseTypes.h>
#include <RowColumn.h>
#include <TextGadget.h>

namespace rose {

    /**
     * class TextSetLayout
     */
    class TextSetLayout : public LinearLayout {
    protected:

    public:
        TextSetLayout() : LinearLayout(MajorAxis::HORIZONTAL) {}
        TextSetLayout(const TextSetLayout&) = delete;
        TextSetLayout(TextSetLayout&&) = default;
        TextSetLayout& operator = (const TextSetLayout&) = delete;
        TextSetLayout& operator = (TextSetLayout&&) = default;

        ~TextSetLayout() override = default;

    };

    /**
     * @class TextSet
     */
    class TextSet : public Row {
    protected:
        constexpr static std::string_view ClassName = "TextSet";

    public:
        TextSet() : Row() { mLayoutManager = std::make_unique<TextSetLayout>(); }
        explicit TextSet(std::shared_ptr<Theme>& theme) : Row(theme) { mLayoutManager = std::make_unique<TextSetLayout>(); }
        TextSet(const TextSet&) = delete;
        TextSet(TextSet &&) = default;
        TextSet& operator=(const TextSet &) = delete;
        TextSet& operator=(TextSet&&) = default;

        ~TextSet() override = default;

        const std::string_view& className() const override { return TextSet::ClassName; }

        [[maybe_unused]] void manage(std::shared_ptr<Gadget> gadget) override;

        [[maybe_unused]] void manage(Builder &builder) override;
    };

    /**
     * @class TextSetBuilder
     */
    class TextSetBuilder : public RowBuilder {
    public:
        [[maybe_unused]] explicit TextSetBuilder(std::shared_ptr<Widget> w) : RowBuilder(std::move(w)) {}

        TextSetBuilder() : RowBuilder(std::make_shared<TextSet>()) {}

        explicit TextSetBuilder(std::shared_ptr<Theme>& themme) : RowBuilder(std::make_shared<TextSet>(themme)) {}

        ~TextSetBuilder() override = default;
    };

} // rose

#endif //ROSE2_TEXTSET_H
