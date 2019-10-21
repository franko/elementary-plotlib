#include <cmath>
#include <random>

#include "libcanvas.h"

using namespace libcanvas;

int main() {
    InitializeFonts();

    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    std::random_device r;
    std::default_random_engine e1(r());
    std::normal_distribution<double> normal_dist(0.0, 0.02);

    const double x0 = 0.0001, x1 = 8 * math::Tau();

    Markers markers{6.0, MarkerSymbol(1)};
    FxLineDraw(markers, x0, x1, [&](double x) { return std::sin(x) / x + normal_dist(e1); }, 23);

    Path line = FxLine(x0, x1, [](double x) { return std::sin(x) / x; });

    plot.AddStroke(line, color::Red, 1.5);
    plot.AddStroke(markers, color::Blue, 1.5);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");

    Window window;
    window.Attach(plot, "");
    window.Start(640, 480, WindowResize);
    window.Wait();
    return 0;
}
