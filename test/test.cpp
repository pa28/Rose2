//
// Created by richard on 31/12/22.
//

#include <iostream>
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

int main(int argc, char **argv) {

    static constexpr MultiButton::ItemArray<4> SpaceButtonItems = {{
           {0u, "rocket"}, {1u, "rocket_launch"}, {2u, "satellite_alt"}, {3u, "satellite"}
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

        if (auto containerBorder = Build<Border>(theme, param::GadgetName{"containerBorder"}, Visual::SHADOW); containerBorder) {
            if (auto container = Build<ButtonBox>(theme, param::GadgetName{"container"},
                              LinearLayout::MajorAxis::VERTICAL, LinearLayout::Alignment::TOP_LEFT); container) {
                if (auto button = Build<Button>(theme); button) {
                    button->activateSignal.connect(buttonSignal);
                    if (auto hello = Build<TextGadget>(theme, param::Text{"Hello"}); hello) {
                        button->manage(hello);
                    }
                    container->manage(button);
                }

                if (auto multi = Build<MultiButton>(theme, param::GadgetName{"worldButton"}, SpaceButtonItems); multi) {
                    multi->updateSignal.connect(multiButtonSignal);
                    container->manage(multi);
                }

                if (auto stateButton =  Build<StateButton>(theme); stateButton) {
                    stateButton->toggleButton();
                    if (auto textSet = Build<TextSet>(theme); textSet) {
                        if (auto check = Build<IconGadget>(theme); check) {
                            textSet->manage(check);
                        }
                        if (auto text = Build<TextGadget>(theme, param::Text{"Check"}); text) {
                            textSet->manage(text);
                        }
                        stateButton->manage(textSet);
                    }
                    container->manage(stateButton);
                }
                containerBorder->manage(container);
            }
            application->manage(containerBorder);
        }

        application->run();
    }

    return 0;
}