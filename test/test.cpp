//
// Created by richard on 31/12/22.
//

#include <iostream>
#include <array>
#include <Rose.h>
#include "manager/RowColumn.h"
#include "manager/Border.h"
#include <TextGadget.h>
#include <Application.h>
#include <Color.h>
#include <Theme.h>
#include "manager/TextSet.h"
#include "buttons/Button.h"
#include "buttons/ButtonBox.h"

using namespace rose;

struct binomial_compare_test {
    ScreenCoordType x, y, w, h;
    std::array<bool,6> result;
};


int main(int argc, char **argv) {

    static constexpr std::array<std::tuple<uint32_t,std::string_view>,4> SpaceButtonItems = {{
            { 0u, "rocket" }, { 1u, "rocket_launch" }, { 2u, "satellite_alt" }, { 3u, "satellite"}
    }};

    if (auto application = std::make_shared<Application>(argc, argv); application) {
        ButtonStateProtocol::slot_type buttonSignal;
        buttonSignal = ButtonStateProtocol::createSlot();
        buttonSignal->receiver = [](bool , uint64_t ) {
            SDL_Event event;
            event.type = SDL_QUIT;
            SDL_PushEvent(&event);
        };

        MultiButtonProtocol::slot_type multiButtonSignal;
        multiButtonSignal = MultiButtonProtocol::createSlot();
        multiButtonSignal->receiver = [](bool state, uint32_t item, uint64_t) {
            fmt::print("Multi button item {} state {}\n", item, state);
        };

        application->initializeGraphics();

        TextGadget::InitializeFontCache("/usr/share/fonts/truetype/liberation2:/usr/share/fonts");

        IconGadget::InitializeMaterial("/home/richard/CLionProjects/material-design-icons/font:/usr/share/fonts:/usr/local/share/fonts",
                                       "MaterialIconsOutlined-Regular");

        auto theme = application->getTheme();
        theme->setThemeShade(HSVA(200.f, .5f, 0.5f, 1.f));
        theme->setThemeColors( color::DarkRed, color::DarkGreen, color::DarkYellow);
        theme->setThemeTextColors( color::DarkRed, color::DarkGreen, color::DarkYellow);
        theme->updateThemeColors();

        application->createWindow(application->applicationName(), Size(800, 480), Point::CenterScreen(1),
                                  SDL_WINDOW_RESIZABLE);

        if (BorderBuilder containerBorder{theme}; containerBorder) {
            containerBorder.visual(Visual::SHADOW).name("containerBorder");
            if (auto container = Build<ButtonBox>(theme, param::GadgetName{"container"},
                              LinearLayout::MajorAxis::VERTICAL, LinearLayout::Alignment::TOP_LEFT); container) {
                if (auto button = Build<Button>(theme); button) {
                    button->activateSignal.connect(buttonSignal);
                    if (auto hello = Build<TextGadget>(theme, param::Text{"Hello"}); hello) {
                        hello >> button;
                    }
                    button >> container;
                }

                if (auto multi = MultiButtonBuilder{theme}; multi) {
                    multi.items(SpaceButtonItems).name("worldBorder");
                    multi.get<MultiButton>()->updateSignal.connect(multiButtonSignal);
                    multi >> container;
                }

                if (auto stateButton =  StateButtonBuilder{theme}; stateButton) {
                    stateButton.radioButton().name("stateButton");
                    if (TextSetBuilder textSet{theme}; textSet) {
                        if (IconGadgetBuilder check{theme}; check) {
                            check >> textSet;
                        }
                        if (TextGadgetBuilder text{theme}; text) {
                            text.text("Check").name("check") >> textSet;
                        }
                        textSet >> stateButton;
                    }
                    stateButton >> container;
                }
                container >> containerBorder.get<Border>();
            }
            containerBorder >> application;
        }

        application->run();
    }

    return 0;
}