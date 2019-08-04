#include "debug_log.h"
#include "libcanvas.h"

using namespace libcanvas;;

int main() {
    InitializeFonts();

    Window window("h.");

    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    Polygon line{{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    plot.Add(line, color::Red, 2.5, color::Yellow, property::Fill | property::Stroke);

    plot.CommitPendingDraw();

    int index = window.Attach(plot, "1");
    window.Start(640, 480, WindowResize);

    sleep(4);

    Polygon line2{{0.8, 1.0}, {0.8, 7.0}, {0.3, 4.0}};
    plot.Add(line2, color::Blue, 2.5, color::None);

    window.SlotRefresh(index);
    plot.CommitPendingDraw();
    window.Wait();
    return 0;
}
