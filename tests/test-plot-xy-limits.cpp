#include <cmath>

#include "libcanvas.h"
#include "libcanvas_utils.h"

using namespace libcanvas;

int main() {
    InitializeFonts();

    Plot plot(Plot::ShowUnits | Plot::AutoLimitsY);
    plot.SetClipMode(true);
    plot.SetXLimits(1, 6 * math::Tau());

    const double x0 = 0.0001, x1 = 8 * math::Tau();
    plot.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");

    Window window;
    window.Attach(plot, "");
    window.Start(640, 480, WindowResize);

    utils::Sleep(2);

    plot.AddStroke(FxLine(0.8, x1, [](double x) { return std::cos(x) / x; }), color::Red, 1.5);
    window.Wait();
    return 0;
}
