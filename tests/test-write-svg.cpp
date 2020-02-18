#include <cmath>

#include "libelplot.h"

using namespace elem;

template <typename Function>
void AddFunction(Plot& plot, double x0, double x1, Function f, Color color, int n = 512) {
    Path line;
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.LineTo(x, f(x));
    }
    plot.AddStroke(std::move(line), color, 1.5);
}

int main() {
    InitializeFonts();

    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    AddFunction(plot, x0, x1, [](double x) { return std::sin(x) / x; }, color::Blue);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");

    AddFunction(plot, 0.8, x1, [](double x) { return std::cos(x) / x; }, color::Red);

    plot.WriteSvg("output.svg", 800, 600);
    return 0;
}
