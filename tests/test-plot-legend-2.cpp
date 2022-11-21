#include <cmath>

#include "elem/elem.h"
using namespace elem;

template <typename Function>
void AddFunction(Plot& p, double x0, double x1, Function f, Color color, int n = 512) {
    Path line;
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.LineTo(x, f(x));
    }
    p.Add(line, color, 1.5, color::None, property::Stroke);
}

Plot CreatePlotLegend() {
    Plot p{Plot::Options{0}};
    p.SetLimits(0.0, -5.0, 16.0, 5.0);
    p.SetClipMode(false);
    p.Add(Path{{0.0, -3.0}, {3.0, -3.0}}, color::Blue, 1.5, color::None, property::Stroke);
    p.Add(Path{{0.0, +3.0}, {3.0, +3.0}}, color::Red,  1.5, color::None, property::Stroke);

    Text label1{"sin(x) / x", 14.0, 0.0, 0.3};
    label1.SetPosition(4.0, -3.0);
    p.Add(label1, color::Black, 1.0, color::Black, property::Fill);

    return p;
}

ELEM_USER_MAIN() {
    InitializeFonts();

    Plot p(Plot::ShowUnits | Plot::AutoLimits);
    p.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    AddFunction(p, x0, x1, [](double x) { return std::sin(x) / x; }, color::Blue);

    p.SetTitle("Function plot example");
    p.SetXAxisTitle("x variable");

    p.SetLegendPlot(CreatePlotLegend(), Plot::Right);

    Window win;
    win.Attach(p, "");
    win.Start(640, 480, WindowResize);

    AddFunction(p, 0.8, x1, [](double x) { return std::cos(x) / x; }, color::Red);

    Plot legend = p.GetLegendPlot(Plot::Right);
    Text cos_label{"cos(x) / x", 14.0, 0.0, 0.3};
    cos_label.SetPosition(4.0, +3.0);
    legend.Add(cos_label, color::Black, 1.0, color::Black, property::Fill);
    p.SetLegendPlot(legend, Plot::Right);

    win.Wait();
    return 0;
}

ELEM_GUI_LOOP()
