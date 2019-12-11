#include <cmath>
#include <memory>

#include "libelplot.h"

using namespace elp;

Plot CreateFunctionPlot(const char *title, unsigned flags) {
    Plot p(Plot::ShowUnits | Plot::AutoLimits);
    p.SetClipMode(false);
    p.SetTitle(title);
    const double x0 = math::Tau() / 4, x1 = 8 * math::Tau();
    auto fsin = [](double x) { return std::sin(x) / x; };
    auto fcos = [](double x) { return std::cos(x) / x; };
    p.AddStroke(FxLine(x0, x1, fsin), color::Blue, 1.5, flags);
    p.AddStroke(FxLine(x0, x1, fcos), color::Red, 1.5, flags);
    return p;
}

int main() {
    InitializeFonts();

    Plot plot1 = CreateFunctionPlot("Plot example (antialias)", property::Stroke);
    Plot plot2 = CreateFunctionPlot("Plot example (outline)", property::Outline);
    Plot plot3 = CreateFunctionPlot("Plot example (aliased)", property::Stroke | property::Crisp);

    Window win("v...");
    win.Attach(plot1, "3");
    win.Attach(plot2, "2");
    win.Attach(plot3, "1");

    win.Start(480, 3 * 320, WindowResize);
    win.Wait();
    return 0;
}
