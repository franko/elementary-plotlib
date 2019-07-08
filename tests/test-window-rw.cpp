#include "libcanvas.h"

int main() {
    libcanvas::InitializeFonts(); // TBD

    libcanvas::Window window;

    libcanvas::Plot plot(libcanvas::Plot::ShowUnits); // TBD ShowUnits
    plot.SetLimits({-1.0, 0.0, 1.0, 10.0});
    plot.SetAxisLabelsAngle(libcanvas::xAxis, 3.141592 / 4);
    plot.EnableLabelFormat(libcanvas::xAxis, "%.6f");

/*    agg::rgba8 none(0,0,0,0);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 yellow(245, 254, 0, 255);
*/

    libcanvas::Polygon line{{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}}; // TBD
    plot.Add(line, libcanvas::color::red, 2.5, libcanvas::color::yellow, libcanvas::property::Fill | libcanvas::property::Stroke);

    plot.CommitPendingDraw(); // TBD

    int index = window.Attach(plot, "");

    window.Start(640, 480, graphics::window_resize); // TBD: window_resize

    sleep(4);

    libcanvas::Polygon line2{{0.8, 1.0}, {0.8, 7.0}, {0.3, 4.0}};
    plot.Add(line2, blue, 2.5, none); // TBD colors with none

    window.SlotRefresh(index);
    plot.CommitPendingDraw(); // TBD

    window.Wait();

    return 0;
}
