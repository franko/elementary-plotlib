#include <cmath>
#include <random>

#include "libcanvas.h"

using namespace libcanvas;

template <typename PathType, typename Function>
void PathFunction(PathType& line, Function f, double x0, double x1, int n = 128) {
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.LineTo(x, f(x));
    }
}

int main() {
    InitializeFonts();

    Window window;
    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    Path line;
    PathFunction(line, [](double x) { return std::sin(x) / x; }, x0, x1, 256);

    std::random_device r;
    std::default_random_engine e1(r());
    std::normal_distribution<double> normal_dist(0.0, 0.02);
    std::normal_distribution<double> normal_dist_large(0.0, 0.08);

    Markers markers{6.0, MarkerSymbol(1)};
    PathFunction(markers, [&](double x) { return std::sin(x) / x + normal_dist(e1); }, x0, x1, 23);

    plot.Add(line, color::Red, 1.5, color::None, property::Stroke);
    plot.Add(markers, color::Blue, 1.5, color::None, property::Stroke);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");

    plot.CommitPendingDraw();

    window.Attach(plot, "");
    window.Start(640, 480, WindowResize);
    window.Wait();
    return 0;
}
