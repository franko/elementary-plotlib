#include <cmath>

#include "FXElemPlotWindow.h"

using namespace elp;

class PlotWindowApp : public FXApp {
    FXDECLARE(PlotWindowApp)
public:
    FXApp(const FXString& name, const FXString& vendor);

    long onElemWindowStart(FXObject *, FXSelector, void *);

    enum {
        ID_PLOT_WINDOW_START = FXApp::ID_LAST,
        ID_LAST,
    };
};

FXDEFMAP(PlotWindowApp) PlotWindowAppMap[] = {
    FXMAPFUNC(SEL_IO_READ, PlotWindowApp::ID_PLOT_WINDOW_START, PlotWindowApp::onElemWindowStart),
};

FXIMPLEMENT(PlotWindowApp,FXApp,PlotWindowAppMap,ARRAYNUMBER(PlotWindowAppMap))

long PlotWindowApp::onElemWindowStart(FXObject *, FXSelector, void *ptr) {
    window_fox_kernel *elem_win = (window_fox_kernel *) ptr;
    auto main_win = new FXMainWindow(this, "Plot Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    auto plot_win = new FXElemPlotWindow(main_win, elem_win, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    elem_win->bind_drawable(plot_win, FXElemPlotWindow::ID_UPDATE_REGION);
    main_win->create();
    main_win->show(PLACEMENT_SCREEN);
    return 1;
}

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

int main(int argc, char *argv[]) {
    PlotWindowApp app("Plot Windows", "libelplot");
    app.init(argc, argv);

    InitializeFonts();

    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    const double x0 = 0.0001, x1 = 8 * math::Tau();
    AddFunction(plot, x0, x1, [](double x) { return std::sin(x) / x; }, color::Blue);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");

    // TODO: create a derivate class of window_gen<window_fox_kernel> that
    // calls on constructor bind_window_environment and remove the newly
    // introduced 'window' method of window_gen.
    auto window_impl_ptr = new window_gen<window_fox_kernel>();
    window_impl_ptr.window().bind_window_environment(&app, PlotWindowApp::ID_PLOT_WINDOW_START);
    Window win(window_impl_ptr);

    app.create();
    return app.run();
}
