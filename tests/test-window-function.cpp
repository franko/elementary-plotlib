#include <cmath>

#include "libcanvas.h"

using namespace libcanvas;

int main() {
    InitializeFonts();

    const double x0 = 0.0001, x1 = 8 * math::Tau();
    Plot plot = Plot{Plot::ShowUnits | Plot::AutoLimits}
        .SetClipMode(false)
        .AddStroke(
            FxLine(x0, x1, [](double x) { return std::sin(x) / x; }),
            color::Blue, 1.5)
        .SetTitle("Function plot example")
        .SetXAxisTitle("x variable")
        .SetYAxisTitle("y variable")
        .AddStroke(
            FxLine(0.8, x1, [](double x) { return std::cos(x) / x; }),
            color::Red, 1.5);

    Window window;
    window
        .Attach(plot, "")
        .Start(640, 480, WindowResize)
        .Wait();
    return 0;
}
