#include <cmath>
#include <utility>

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

Legend CreateLegend(bool include_cos) {
    Legend legend;

    Group sin_entry;
    sin_entry.AddStroke(Path{{0.0, 0.0}, {24.0, 0.0}}, color::Blue, 1.5);
    Text sin_label{"sin(x) / x", 14.0};
    sin_label.SetPosition(32.0, 0.0);
    sin_entry.Add(sin_label, color::Black, 0.0, color::Black, property::Fill);
    legend.Add(std::move(sin_entry));

    if (include_cos) {
        Group cos_entry;
        cos_entry.AddStroke(Path{{0.0, 0.0}, {24.0, 0.0}}, color::Red, 1.5);
        Text cos_label{"cos(x) / x", 14.0};
        cos_label.SetPosition(32.0, 0.0);
        cos_entry.Add(cos_label, color::Black, 0.0, color::Black, property::Fill);
        legend.Add(std::move(cos_entry));
    }

    return legend;
}

ELEM_USER_MAIN() {
    InitializeFonts();

    Plot p(Plot::ShowUnits | Plot::AutoLimits);
    p.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    AddFunction(p, x0, x1, [](double x) { return std::sin(x) / x; }, color::Blue);

    p.SetTitle("Function plot example");
    p.SetXAxisTitle("x variable");

    p.SetLegend(CreateLegend(false), LegendLocation::Right);

    Window win;
    win.Attach(p, "");
    win.Start(640, 480, WindowResize);

    AddFunction(p, 0.8, x1, [](double x) { return std::cos(x) / x; }, color::Red);

    p.SetLegend(CreateLegend(true), LegendLocation::Right);

    win.Wait();
    return 0;
}

ELEM_GUI_LOOP()
