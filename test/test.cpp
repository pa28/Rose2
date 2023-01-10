//
// Created by richard on 31/12/22.
//

#include <iostream>
#include <array>
#include <Rose.h>
#include <RowColumn.h>
#include <TextGadget.h>
#include <Application.h>
#include <Color.h>
#include <Theme.h>

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
        Theme &theme{application->getTheme()};
        theme.setThemeShade(Color{ 0.2, 0.7, 0.7, 1.0});
        theme.textForeground = theme.shades[Theme::IMPRINT];
        theme.screenBackground = theme.shades[Theme::DN_BG];
        theme.gadgetBackground = theme.shades[Theme::UP_BG];

        application->createWindow(application->applicationName(), Size(800, 480), Point::CenterScreen(1),
                                  SDL_WINDOW_RESIZABLE);

        if (auto container = ColumnBuilder{}; container) {
            container.setLayoutAlignment(LinearLayout::Alignment::BOTTOM_RIGHT)
                    .name("container")
                    .background(theme.gadgetBackground)
                    .layout(Point(0,0), Size(10,10));

            if (TextGadgetBuilder hello{}; hello) {
                hello.text("Hello")
                        .pointSize(20)
                        .foreground(theme.textForeground)
                        .background(theme.gadgetBackground)
                        .layout(Point(0,0),Size(10,10))
                        .name("Hello")
                        .decorator(backgroundDecorator)
                        >> container;
                std::cout << "Hello\n";
            }

            if (TextGadgetBuilder world{}; world) {
                world.text("World")
                        .pointSize(20)
                        .foreground(theme.textForeground)
                        .background(theme.gadgetBackground)
                        .layout(Point(0,0),Size(10,10))
                        .name("World")
                        .decorator(backgroundDecorator)
                        >> container;
                std::cout << "World\n";
            }

            container >> application;
        }

        application->run();
    }

    return 0;
}