//
// Created by richard on 31/12/22.
//

#include <iostream>
#include <vector>
#include <array>
#include <tuple>
#include <Rose.h>
#include <Gadget.h>
#include <TextGadget.h>
#include <Application.h>
#include <Color.h>

using namespace rose;

struct binomial_compare_test {
    ScreenCoordType x, y, w, h;
    std::array<bool,6> result;
};

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

int main(int argc, char **argv) {
//    int pass{0}, total{0};
//
//    auto [testTotal,testPass] = screen_obj_compare_test(binomial_compare_test_1);
//    pass += testPass;
//    total += testTotal;
//
//    std::cout << "Pass: " << pass << ", Total: " << total << '\n';
//
//    auto gadget = std::make_shared<Gadget>();
//    [[maybe_unused]] auto gadgetType = gadget->gadgetType();
//
//    auto widget = std::make_shared<Widget>();
//    [[maybe_unused]] auto widgetType = widget->gadgetType();
//
//    widget->manage(std::move(gadget));
//
//    auto widget1 = std::make_shared<Widget>();
//    widget1->manage(widget->front());

    Application application(argc, argv);
    application.initializeGraphics();
    application.createWindow(application.applicationName(), Size(800,480), Point::CenterScreen(1), 0);

    auto container = WidgetBuilder{}.name(application.applicationName())
            .layout(Point(0,0), Size(800,480))
            .background(color::TransparentBlack);

    if (TextGadgetBuilder b{}; b) {
        b.name("Text");
        b.layout(Point(0,0), Size(10,10));
        b.foreground(color::OpaqueWhite).text("Hello").pointSize(30);
        b.background(Color( 0.5, 0.0, 0.0, 1.0)) >> container;
    }

    if (GadgetBuilder g{}; g) {
        g.name("Map").layout(Point(140,150),Size(660,330));
        g.background(Color(0.0, 0.5, 0.0, 1.0)) >> container;
    }


    application.begin()->get()->emplace_back(container.get<Widget>());
    application.run();

    return 0;
}