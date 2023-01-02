//
// Created by richard on 31/12/22.
//

#include <iostream>
#include <vector>
#include <array>
#include <tuple>
#include <Rose.h>
#include <Gadget.h>
#include <Application.h>

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
    application.run();
//    mGraphicsModel.initialize("Test", Size{300,300}, Point{100, 100}, 0);
//    mGraphicsModel.eventLoop();

    return 0;
}