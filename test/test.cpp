//
// Created by richard on 31/12/22.
//

#include <iostream>
#include <Rose.h>
#include <Build.h>
#include "manager/RowColumn.h"
#include "manager/Border.h"
#include <TextGadget.h>
#include <Application.h>
#include <Color.h>
#include <Theme.h>
#include "buttons/Button.h"
#include "buttons/ButtonBox.h"
#include <buttons/PushButton.h>

using namespace rose;

int main(int argc, char **argv) {

    [[maybe_unused]] static constexpr MultiButton::ItemArray<4> SpaceButtonItems = {{
                                                                                            {0u, "rocket"},
                                                                                            {1u, "rocket_launch"},
                                                                                            {2u, "satellite_alt"},
                                                                                            {3u, "satellite"}
                                                                                    }};

    if (auto application = std::make_shared<Application>(argc, argv); application) {
        ButtonStateProtocol::slot_type buttonSignal;
        buttonSignal = ButtonStateProtocol::createSlot();
        buttonSignal->receiver = [](bool, uint64_t) {
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

        IconGadget::InitializeMaterial(
                "/home/richard/CLionProjects/material-design-icons/font:/usr/share/fonts:/usr/local/share/fonts",
                "MaterialIconsOutlined-Regular");

        auto theme = application->getTheme();
        theme->setThemeShade(HSVA(200.f, .5f, 0.5f, 1.f));
        theme->setThemeColors(color::DarkRed, color::DarkGreen, color::DarkYellow);
        theme->setThemeTextColors(color::DarkRed, color::DarkGreen, color::DarkYellow);
        theme->updateThemeColors();

        application->createWindow(application->applicationName(), Size(800, 480), Point::CenterScreen(1),
                                  SDL_WINDOW_RESIZABLE);

        application->manage(Build<Border>(theme, param::GadgetName{"containerBorder"}, Visual::SHADOW)->manage(
                Build<ButtonBox>(theme, param::GadgetName{"buttonBox"},
                                 LinearLayout::MajorAxis::VERTICAL,
                                 LinearLayout::Alignment::TOP_LEFT)->manageAll(
                        Build<LabelButton>(theme, param::Text{"Hello"}, param::GadgetName{"hello"},
                                           param::ActivateSignal{buttonSignal}),
                        Build<BadgedButton>(theme, param::Icon{"wifi"}, param::Text{"Wifi"}, param::GadgetName{"WiFi"}),
                        Build<MultiButton>(theme, param::GadgetName{"space"}, SpaceButtonItems,
                                           MultiButton::UpdateSignal{multiButtonSignal}),
                        RadioButton(theme, param::Text{"Radio"}),
                        ToggleButton(theme, param::Text{"Toggle"}),
                        CheckButton(theme, param::Text{"Check"})
                )
        ));

        application->run();
    }

    return 0;
}