#include <cmath>

#include "libelplot.h"

using namespace elp;

int main() {
    InitializeFonts();

    Window window;

    const double x0 = 0.0001, x1 = 8 * math::Tau();
    Plot plot1;
    // plot.SetClipMode(false);
    plot1.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);
    plot1.SetTitle("y = sin(x) / x");
    plot1.SetXAxisTitle("x variable");
    plot1.SetYAxisTitle("y variable");

    Plot plot2;
    plot2.AddStroke(FxLine(0.8, x1, [](double x) { return std::cos(x) / x; }), color::Red, 1.5);
    plot2.SetTitle("y = cos(x) / x");
    plot2.SetXAxisTitle("x variable");
    plot2.SetYAxisTitle("y variable");

    window.SetLayout("v..");
    window.Attach(plot1, "1");
    window.Attach(plot2, "2");
    window.Start(640, 480, WindowResize);
    window.Wait();
    return 0;
}
