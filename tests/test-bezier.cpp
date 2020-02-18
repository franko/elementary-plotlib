#include "libelplot_utils.h"
#include "libelplot.h"

using namespace elem;;

int main() {
    InitializeFonts();

    Plot plot(Plot::ShowUnits);
    plot.SetLimits(0.0, 0.0, 100.0, 100.0);

    CurvePath heart;
    heart.MoveTo(10, 70);
    heart.ArcTo(20, 20, 0, false, false, 50, 70);
    heart.ArcTo(20, 20, 0, false, false, 90, 70);
    heart.Curve3(90, 40, 50, 10);
    heart.Curve3(10, 40, 10, 70);
    heart.ClosePolygon();

    plot.Add(heart, color::Red, 2.5, color::RGB(0xD4, 0x40, 0x40));

    Window window;
    window.Attach(plot, "");
    window.Start(480, 480, WindowResize);
    window.Wait();
    return 0;
}
