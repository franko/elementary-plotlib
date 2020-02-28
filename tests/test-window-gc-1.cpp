#include <cmath>

#include "elem_plot_utils.h"
#include "elem_plot.h"

using namespace elem;

static void CreatePlotWindow() {
    const double x0 = 0.0001, x1 = 8 * math::Tau();
    Plot plot1;
    plot1.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);
    plot1.SetTitle("y = sin(x) / x");
    plot1.SetXAxisTitle("x variable");
    plot1.SetYAxisTitle("y variable");

    Plot plot2;
    plot2.AddStroke(FxLine(0.1, x1, [](double x) { return std::cos(x) / x; }), color::Red, 1.5);
    plot2.SetTitle("y = cos(x) / x");
    plot2.SetXAxisTitle("x variable");
    plot2.SetYAxisTitle("y variable");

    Window window1("v..");
    window1.Attach(plot1, "1");
    window1.Attach(plot2, "2");

    Window window2;
    window2.Attach(plot2, "");

    window1.Start(640, 480, WindowResize);
    window2.Start(640, 480, WindowResize);
}

int main() {
    InitializeFonts();
    CreatePlotWindow();
    utils::Sleep(8);
    return 0;
}
