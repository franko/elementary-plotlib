#include <cmath>
#include <memory>

#include "libcanvas.h"

using namespace libcanvas;

template <typename Function>
Object NewLineFunction(double x0, double x1, Function f, int n = 512) {
    Path line;
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.LineTo(x, f(x));
    }
    return line;
}

std::unique_ptr<Plot> CreateFunctionPlot(const char *title, unsigned flags) {
    auto p = std::make_unique<Plot>(Plot::ShowUnits | Plot::AutoLimits);
    p->SetClipMode(false);
    p->SetTitle(title);
    const double x0 = 3.14159265358979323846/2, x1 = 8 * 2 * 3.14159265358979323846;
    auto fsin = [](double x) { return std::sin(x) / x; };
    auto fcos = [](double x) { return std::cos(x) / x; };
    p->Add(NewLineFunction(x0, x1, fsin), color::Blue, 1.5, color::None, flags);
    p->Add(NewLineFunction(x0, x1, fcos), color::Red, 1.5, color::None, flags);
    p->CommitPendingDraw();
    return p;
}

int main() {
    InitializeFonts();

    Window win("v...");

    auto plot1 = CreateFunctionPlot("Plot example (antialias)", property::Stroke);
    auto plot2 = CreateFunctionPlot("Plot example (outline)", property::Outline);
    auto plot3 = CreateFunctionPlot("Plot example (aliased)", property::Stroke | property::Crisp);

    win.Attach(*plot1, "3");
    win.Attach(*plot2, "2");
    win.Attach(*plot3, "1");

    win.Start(480, 3 * 320, WindowResize);
    win.Wait();
    return 0;
}
