#include "libelplot_utils.h"
#include "libelplot.h"

using namespace elp;;

int main() {
    InitializeFonts();

    Plot *plot = new Plot(Plot::ShowUnits);
    plot->SetLimits({-1.0, 0.0, 1.0, 10.0});
    plot->SetAxisLabelsAngle(xAxis, 3.141592 / 4);
    plot->EnableLabelFormat(xAxis, "%.6f");

    Polygon line{{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    plot->Add(line, color::Red, 2.5, color::Yellow, property::Fill | property::Stroke);

    Polygon line2{{0.8, 1.0}, {0.8, 7.0}, {0.3, 4.0}};
    plot->Add(line2, color::Blue, 2.5, color::None);

    Plot plot2 = *plot;
    delete plot;

    Window window;
    window.Attach(plot2, "");
    window.Start(640, 480, WindowResize);
    window.Wait();
    return 0;
}
