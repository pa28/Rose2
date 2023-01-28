//
// Created by richard on 20/01/23.
//

/*
 * TextSet.cpp Created by Richard Buckley (C) 20/01/23
 */

/**
 * @file TextSet.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 20/01/23
 */

#include "manager/TextSet.h"

namespace rose {
    void TextSet::manage(std::shared_ptr<Gadget> gadget) {
        if (auto text = std::dynamic_pointer_cast<TextGadget>(gadget); text)
            Row::manage(gadget);
        else
            throw SceneTreeError(fmt::format("TextSet can not manage a {}", gadget->className()));
    }

    bool TextSetLayout::initialWidgetLayout(Context &context, std::shared_ptr<Gadget> &managingGadget) {
        auto result = LinearLayout::initialWidgetLayout(context, managingGadget);
        std::shared_ptr<Gadget> mainGadget;
        ScreenCoordType maxHeight{};
        GlyphMetrics mainGlyph{};
        if (auto widget = std::dynamic_pointer_cast<Widget>(managingGadget)) {
            for (auto &gadget: getGadgetList(widget)) {
                if (auto textGadget = std::dynamic_pointer_cast<TextGadget>(gadget); textGadget) {
                    auto code = uint32_t('M');
                    if (auto iconGadget = std::dynamic_pointer_cast<IconGadget>(gadget); iconGadget)
                        code = iconGadget->getIconCode();
                    auto gm = getGlyphMetrics32(textGadget->getFont(), code);
                    if (auto height = gadget->getVisualMetrics().desiredSize.h; height > maxHeight) {
                        maxHeight = height;
                        mainGadget = gadget;
                        mainGlyph = gm;
                    }
                }
            }

            for (auto &gadget: getGadgetList(widget)) {
                if (gadget != mainGadget) {
                    if (auto textGadget = std::dynamic_pointer_cast<TextGadget>(gadget); textGadget) {
                        if (gadget->getVisualMetrics().desiredSize.h < maxHeight) {
                            if (auto iconGadget = std::dynamic_pointer_cast<IconGadget>(gadget); iconGadget) {
                                auto gm = getGlyphMetrics32(textGadget->getFont(), iconGadget->getIconCode());
                                gadget->getVisualMetrics().innerAlignmentPadding =
                                        Padding{0, (mainGlyph.fontMetrics.fontLineSkip - gm.fontMetrics.fontLineSkip)/2, 0, 0};
                            } else {
                                auto gm = getGlyphMetrics32(textGadget->getFont(), uint32_t('M'));
                                auto tPad = (gm.minY - mainGlyph.minY) + (mainGlyph.fontMetrics.fontAscent - gm.fontMetrics.fontAscent);
                                gadget->getVisualMetrics().innerAlignmentPadding = Padding{0, tPad, 0, 0};
                            }
                            gadget->immediateGadgetLayout();
                        }
                    }
                }
            }
        }
        return result;
    }

} // rose