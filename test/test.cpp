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

#if 0
std::vector<binomial_compare_test> binomial_compare_test_1 = {{
    {0, 0, 0, 0, {false, true, true, false, true, false} },
    {0, 0, 1, 0, {true, true, false, true, false, false} },
    {0, 0, 0, 1, {true, true, false, true, false, false} },
    {0, 0, 1, 1, {true, true, false, true, false, false} },
    {0, 1, 0, 0, {false, false, false, true, true, true} },
    {1, 0, 0, 0, {false, false, false, true, true, true} },
    {1, 1, 0, 0, {false, false, false, true, true, true} },
}};

std::tuple<int,int> screen_obj_compare_test(const std::vector<binomial_compare_test>& tests) {
    int pass{0};
    int total{0};
    for (const auto &test : tests) {
        Point p0(test.x, test.y), p1(test.w, test.h);
        if ((p0 < p1) == test.result[0]) ++pass; else std::cout << "Fail " << p0 << '<' << p1 << '\n';
        if ((p0 <= p1) == test.result[1]) ++pass; else std::cout << "Fail " << p0 << "<=" << p1 << '\n';
        if ((p0 == p1) == test.result[2]) ++pass; else std::cout << "Fail " << p0 << "==" << p1 << '\n';
        if ((p0 != p1) == test.result[3]) ++pass; else std::cout <<  "Fail " << p0 << "!=" << p1 << '\n';
        if ((p0 >= p1) == test.result[4]) ++pass; else std::cout << "Fail " << p0 << ">=" << p1 << '\n';
        if ((p0 > p1) == test.result[5]) ++pass; else std::cout << "Fail " << p0 << '>' << p1 << '\n';
        total += 6;
    }

    for (const auto &test : tests) {
        Size p0(test.x, test.y), p1(test.w, test.h);
        if ((p0 < p1) == test.result[0]) ++pass; else std::cout << "Fail " << p0 << '<' << p1 << '\n';
        if ((p0 <= p1) == test.result[1]) ++pass; else std::cout << "Fail " << p0 << "<=" << p1 << '\n';
        if ((p0 == p1) == test.result[2]) ++pass; else std::cout << "Fail " << p0 << "==" << p1 << '\n';
        if ((p0 != p1) == test.result[3]) ++pass; else std::cout <<  "Fail " << p0 << "!=" << p1 << '\n';
        if ((p0 >= p1) == test.result[4]) ++pass; else std::cout << "Fail " << p0 << ">=" << p1 << '\n';
        if ((p0 > p1) == test.result[5]) ++pass; else std::cout << "Fail " << p0 << '>' << p1 << '\n';
        total += 6;
    }

    return std::make_tuple(total, pass);
}
#endif

int main(int argc, char **argv) {

    if (auto application = std::make_shared<Application>(argc, argv); application) {
        application->initializeGraphics();
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
                        hello.text("Hello").name("Hello") >> border;
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
                        if (IconGadgetBuilder icon{theme}; icon) {
                            icon.icon(MATERIAL_settings) >> textSet;
                        }
                        if (TextGadgetBuilder text{theme}; text) {
                            text.text("Ex\u0332it") >> textSet;
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