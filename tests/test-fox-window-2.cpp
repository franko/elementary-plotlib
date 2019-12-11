#include <cmath>
#include <thread>

#include "FXElpWindow.h"
#include "libelplot_utils.h"

using namespace elp;

void RunFox(FXApp *app, FXMainWindow *win) {
    app->create();
    win->show(PLACEMENT_SCREEN);
    app->run();
    delete app;
}

int main(int argc, char *argv[]) {
    InitializeFonts();

    auto app = new FXApp("libelplot", "libelplot");
    app->init(argc, argv);

    auto main_window = new FXMainWindow(app, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    auto window = new FXElpWindow(main_window, nullptr, LAYOUT_FILL_X|LAYOUT_FILL_Y);

    Plot p(Plot::ShowUnits);
    p.SetLimits({-1.0, 0.0, 1.0, 10.0});
    p.SetAxisLabelsAngle(xAxis, 3.141592 / 4);
    p.EnableLabelFormat(yAxis, "%.6f");

    Polygon line{{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    p.Add(line, color::Red, 2.5, color::Yellow, property::Fill | property::Stroke);

    p.CommitPendingDraw();

    int index = window->Attach(p, "");

    std::thread wt(RunFox, app, main_window);
    wt.detach();

    utils::Sleep(4);

    Polygon line2{{0.8, 1.0}, {0.8, 7.0}, {0.3, 4.0}};
    p.Add(line2, color::Blue, 2.5, color::None);

    window->SlotRefresh(index);
    p.CommitPendingDraw();
    window->Wait();
    return 0;
}
