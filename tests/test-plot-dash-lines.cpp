#include <cmath>

#include "elem/elem.h"

using namespace elem;

int main() {
    InitializeFonts();

    Plot plot;

    const double x0 = 0.0001, x1 = 8 * math::Tau();
    DashPath line{8, 4};
    FxLineDraw(line, x0, x1, [](double x) { return std::sin(x) / x; });
    plot.AddStroke(std::move(line), color::Blue, 1.5);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");

    Window window;
    window.Attach(plot, "");
    window.Start(640, 480, WindowResize);

    DashPath line2{8, 4, 2, 4};
    FxLineDraw(line2, 0.8, x1, [](double x) { return std::cos(x) / x; });
    plot.AddStroke(std::move(line2), color::Red, 1.5);

    window.Wait();
    return 0;
}
