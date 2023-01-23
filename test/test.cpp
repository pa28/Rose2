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

using namespace rose;

struct binomial_compare_test {
    ScreenCoordType x, y, w, h;
    std::array<bool,6> result;
};


int main(int argc, char **argv) {

    if (auto application = std::make_shared<Application>(argc, argv); application) {
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

                if (BorderBuilder border{theme}; border) {
                    border.name("helloBorder");
                    if (TextGadgetBuilder hello{theme}; hello) {
                        hello.text("Hello").name("Hello").lightColor(color::DarkYellow.color()) >> border;
                        std::cout << "hello\n";
                    }
                    border >> container;
                }

                if (BorderBuilder border{theme}; border) {
                    border.name("worldBorder");
                    if (TextGadgetBuilder world{theme}; world) {
                        world.text("World").name("world") >> border;
                        std::cout << "World\n";
                    }
                    border >> container;
                }

                if (BorderBuilder border{theme}; border) {
                    border.name("setBorder");
                    if (TextSetBuilder textSet{theme}; textSet) {
                        if (TextGadgetBuilder text{theme}; text) {
                            text.text("Exit") >> textSet;
                        }
                        if (IconGadgetBuilder cmd{theme}; cmd) {
                            cmd.icon(60135)
//                                .animation<LightAnimation>()
                                .name("icon")
                                .lightColor(color::DarkRed.color())
                                     >> textSet;
                            cmd.get<Gadget>()->mAnimationPtr = std::make_unique<LightAnimation>();
                            cmd.get<Gadget>()->mAnimationPtr->setGadget(cmd.get<Gadget>());
                        }
                        if (TextGadgetBuilder shortCut{theme}; shortCut) {
                            shortCut.text("x") >> textSet;
                        }
                        textSet >> border;
                    }
                    border >> container;
                }
                container >> containerBorder;
            }
            containerBorder >> application;
        }

        application->run();
    }

    return 0;
}