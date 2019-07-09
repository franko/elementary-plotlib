#include "debug_log.h"
#include "libcanvas.h"

int main() {
    libcanvas::InitializeFonts();

    libcanvas::Window window;

    libcanvas::Plot plot(libcanvas::Plot::ShowUnits); // TBD ShowUnits
    plot.SetLimits({-1.0, 0.0, 1.0, 10.0});
    plot.SetAxisLabelsAngle(libcanvas::xAxis, 3.141592 / 4);
    plot.EnableLabelFormat(libcanvas::xAxis, "%.6f");

    libcanvas::Polygon line{{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    plot.Add(line, libcanvas::color::Red, 2.5, libcanvas::color::Yellow, libcanvas::property::Fill | libcanvas::property::Stroke);

    plot.CommitPendingDraw();
    int index = window.Attach(plot, "");

    window.Start(640, 480, libcanvas::WindowResize);
    sleep(4);

    libcanvas::Polygon line2{{0.8, 1.0}, {0.8, 7.0}, {0.3, 4.0}};
    plot.Add(line2, libcanvas::color::Blue, 2.5, libcanvas::color::None);

    window.SlotRefresh(index);
    plot.CommitPendingDraw();
    window.Wait();
    return 0;
}
