#include <cmath>
#include <thread>

#include "FXElpWindow.h"

using namespace elp;

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
    delete app;
}

int main(int argc, char *argv[]) {
    InitializeFonts();

    auto app = new FXApp("libelplot", "libelplot");
    app->init(argc, argv);

    auto main_window = new FXMainWindow(app, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    auto window = new FXElpWindow(main_window, nullptr, LAYOUT_FILL_X|LAYOUT_FILL_Y);

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
