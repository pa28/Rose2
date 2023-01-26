//
// Created by richard on 31/12/22.
//

#include <iostream>
#include <array>
#include <Rose.h>
#include <RowColumn.h>
#include <Border.h>
#include <TextGadget.h>
#include <Application.h>
#include <Color.h>
#include <Theme.h>
#include <TextSet.h>
#include <Button.h>

using namespace rose;

struct binomial_compare_test {
    ScreenCoordType x, y, w, h;
    std::array<bool,6> result;
};


int main(int argc, char **argv) {

    if (auto application = std::make_shared<Application>(argc, argv); application) {
        ButtonStateProtocol::slot_type buttonSignal;
        buttonSignal = ButtonStateProtocol::createSlot();
        buttonSignal->receiver = [](bool , uint64_t ) {
            SDL_Event event;
            event.type = SDL_QUIT;
            SDL_PushEvent(&event);
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
            if (auto container = ColumnBuilder{}; container) {
                container.setLayoutAlignment(LinearLayout::Alignment::TOP_LEFT)
                        .name("container");

                if (auto button = ButtonBuilder{theme}; button) {
                    button.name("helloButton");
                    button.get<Button>()->activateSignal.connect(buttonSignal);
                    if (TextGadgetBuilder hello{theme}; hello) {
                        hello.text("Hello").name("Hello") >> button;
                        std::cout << "hello\n";
                    }
                    button >> container;
                }

                if (BorderBuilder border{theme}; border) {
                    border.name("worldBorder");
                    if (TextGadgetBuilder world{theme}; world) {
                        world.text("World").name("world") >> border;
                        std::cout << "World\n";
                    }
                    border >> container;
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
                container >> containerBorder;
            }
            containerBorder >> application;
        }

        application->run();
    }

    return 0;
}