#include <cmath>
#include <thread>

#include "libcanvas.h"
#include "debug_log.h"
#include "fox/GraphicsWindow.h"

using namespace libcanvas;

template <typename Function>
void AddFunction(Plot& plot, double x0, double x1, Function f, Color color, int n = 512) {
    Path line;
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.LineTo(x, f(x));
    }
    plot.Add(std::move(line), color, 1.5, color::None, property::Stroke);
}

void RunFox(FXApp *app, FXMainWindow *win) {
    app->create();
    win->show(PLACEMENT_SCREEN);
    app->run();
    debug_log("thread terminating...");
    delete app;
    debug_log("app terminated...");
}

int main(int argc, char *argv[]) {
    InitializeFonts();

    auto app = new FXApp("libcanvas", "libcanvas");
    app->init(argc, argv);

    auto main_window = new FXMainWindow(app, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    auto window = new GraphicsWindow(main_window, LAYOUT_FILL_X|LAYOUT_FILL_Y);

    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    AddFunction(plot, x0, x1, [](double x) { return std::sin(x) / x; }, color::Blue);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.CommitPendingDraw();

    window->Attach(plot, "");

    std::thread wt(RunFox, app, main_window);
    wt.detach();
    window->Wait();

    return 0;
}
