#include <cmath>

#include "libcanvas.h"

using namespace libcanvas;

template <typename Function>
void SetFunctionPath(Path& line, double x0, double x1, Function f, int n = 512) {
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.LineTo(x, f(x));
    }
}

int main() {
    InitializeFonts();

    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    DashPath line{8, 4};
    SetFunctionPath(line, x0, x1, [](double x) { return std::sin(x) / x; });
    plot.AddStroke(std::move(line), color::Blue, 1.5);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");

    Window window;
    window.Attach(plot, "");
    window.Start(640, 480, WindowResize);

    DashPath line2{8, 4, 2, 4};
    SetFunctionPath(line2, 0.8, x1, [](double x) { return std::cos(x) / x; });
    plot.AddStroke(std::move(line2), color::Red, 1.5);

    window.Wait();
    return 0;
}
