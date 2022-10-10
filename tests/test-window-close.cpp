#include <cstdio>
#include "elem/elem_utils.h"
#include "elem/elem.h"

using namespace elem;

ELEM_USER_MAIN() {
    InitializeFonts();

    Plot plot(Plot::ShowUnits);
    plot.SetLimits(-1.0, 0.0, 1.0, 10.0);
    plot.SetAxisLabelsAngle(xAxis, 3.141592 / 4);
    plot.EnableLabelFormat(xAxis, "%.6f");

    Polygon line{{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    plot.Add(line, color::Red, 2.5, color::Yellow, property::Fill | property::Stroke);

    Window window;
    window.Attach(plot, "");

    window.Start(640, 480, WindowResize);
    utils::Sleep(4);
    window.Close();

    fprintf(stderr, "window close message sent, waiting 2 seconds.\n");
    utils::Sleep(2);
    return 0;
}

ELEM_GUI_LOOP()
