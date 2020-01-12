#include <cmath>

#include "libelplot.h"
#include "FXElemPlotWindow.h"
#include "window_fox_kernel.h"
#include "window_gen.h"

using namespace elp;

class PlotWindow : public FXMainWindow {
    FXDECLARE(PlotWindow)
protected:
    PlotWindow() { }
private:
    PlotWindow(const PlotWindow&);
    PlotWindow &operator=(const PlotWindow&);
public:
    PlotWindow(FXApp* a, const FXString& name, FXIcon *ic=NULL, FXIcon *mi=NULL, FXuint opts=DECOR_ALL,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=0,FXint pr=0,FXint pt=0,FXint pb=0,FXint hs=0,FXint vs=0):
        FXMainWindow(a, name, ic, mi, opts, x, y, w, h, pl, pr, pt, pb, hs, vs) {
    }

    long onElemWindowStart(FXObject *, FXSelector, void *);
    long onNewWindowCmd(FXObject *, FXSelector, void *);

    enum {
        ID_PLOT_WINDOW_START = FXApp::ID_LAST,
        ID_NEW_WINDOW,
        ID_LAST,
    };
};

FXDEFMAP(PlotWindow) PlotWindowMap[] = {
    FXMAPFUNC(SEL_IO_READ, PlotWindow::ID_PLOT_WINDOW_START, PlotWindow::onElemWindowStart),
    FXMAPFUNC(SEL_COMMAND, PlotWindow::ID_NEW_WINDOW, PlotWindow::onNewWindowCmd),
};

FXIMPLEMENT(PlotWindow, FXMainWindow, PlotWindowMap, ARRAYNUMBER(PlotWindowMap))

long PlotWindow::onElemWindowStart(FXObject *, FXSelector, void *ptr) {
    fprintf(stderr, "PlotWindow::onElemWindowStart\n");
    window_fox_kernel *elem_win = (window_fox_kernel *) ptr;
    FXApp *app = getApp();
    auto main_win = new FXMainWindow(app, "Plot Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    auto plot_win = new FXElemPlotWindow(main_win, elem_win, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    elem_win->bind_drawable(plot_win, FXElemPlotWindow::ID_UPDATE_REGION);
    main_win->create();
    main_win->show(PLACEMENT_SCREEN);
    return 1;
}

long PlotWindow::onNewWindowCmd(FXObject *, FXSelector, void *) {
    // TODO: create a derivate class of window_gen<window_fox_kernel> that
    // calls on constructor bind_window_environment and remove the newly
    // introduced 'window' method of window_gen.
    auto window_impl_ptr = new window_gen<window_fox_kernel>();
    window_impl_ptr->window().bind_window_environment(getApp(), this, PlotWindow::ID_PLOT_WINDOW_START);
    Window win(window_impl_ptr);
    // FIX the handling of width, height
    win.Start(0, 0, 0);
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

Plot CreateNewPlot() {
    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);
    const double x0 = 0.0001, x1 = 8 * math::Tau();
    AddFunction(plot, x0, x1, [](double x) { return std::sin(x) / x; }, color::Blue);
    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    return plot;
}

int main(int argc, char *argv[]) {
    FXApp app("Plot Windows", "libelplot");
    app.init(argc, argv);
    InitializeFonts();
    auto main_window = new PlotWindow(&app, "FOX Window host example", nullptr, nullptr, DECOR_ALL, 0, 0, 320, 320);
    new FXButton(main_window, "New Plot", nullptr, main_window, PlotWindow::ID_NEW_WINDOW);
    app.create();
    main_window->show(PLACEMENT_SCREEN);
    return app.run();
}
