#include <cmath>

#include "libcanvas.h"

using namespace libcanvas;

template <typename Function>
void AddFunction(Plot& plot, double x0, double x1, Function f, Color color, int n = 512) {
    Path line;
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.LineTo(x, f(x));
    }
    plot.Add(line, color, 1.5, color::None, property::Stroke);
}

int main() {
    InitializeFonts();

    Window window;
    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    AddFunction(plot, x0, x1, [](double x) { return std::sin(x) / x; }, color::Blue);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");

    plot.CommitPendingDraw();

    int index = window.Attach(plot, "");

    window.Start(640, 480, WindowResize);

    AddFunction(plot, 0.8, x1, [](double x) { return std::cos(x) / x; }, color::Red);
    window.SlotRefresh(index);
    plot.CommitPendingDraw();

    window.Wait();

    return 0;
}
