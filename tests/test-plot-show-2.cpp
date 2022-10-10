#include <cmath>

#include "elem/elem.h"
#include "elem/elem_utils.h"

using namespace elem;

ELEM_USER_MAIN() {
    Plot plot;
    const double x0 = 0.0001, x1 = 8 * math::Tau();
    plot.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);
    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");
    plot.Show(640, 480, WindowResize);
    plot.Show(640, 640, WindowResize);
    plot.Show(480, 480, WindowResize);
    plot.AddStroke(FxLine(0.8, x1, [](double x) { return std::cos(x) / x; }), color::Red, 1.5);
    plot.Wait();
    utils::Sleep(6);
    return 0;
}

ELEM_GUI_LOOP()
