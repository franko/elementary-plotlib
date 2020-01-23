#include <cmath>
#include <thread>

#include "libelplot.h"
#include "libelplot_utils.h"
#include "FXElemBaseWindow.h"
#include "window_platform_fox.h"

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
        start_signal = new FXGUISignal(a, this, ID_PLOT_WINDOW_START, nullptr);
        frame = new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y);
        new FXLabel(frame, "Window Plot demonstrator");
    }

    ~PlotWindow() {
        delete start_signal;
    }

    long onElemWindowStart(FXObject *, FXSelector, void *);

    enum {
        ID_PLOT_WINDOW_START = FXApp::ID_LAST,
        ID_LAST,
    };

    FXVerticalFrame *frame;
    FXGUISignal *start_signal;
};

FXDEFMAP(PlotWindow) PlotWindowMap[] = {
    FXMAPFUNC(SEL_IO_READ, PlotWindow::ID_PLOT_WINDOW_START, PlotWindow::onElemWindowStart),
};

FXIMPLEMENT(PlotWindow, FXMainWindow, PlotWindowMap, ARRAYNUMBER(PlotWindowMap))

long PlotWindow::onElemWindowStart(FXObject *, FXSelector, void *ptr) {
    FXElemStartMessage *message = (FXElemStartMessage *) ptr;
    if (!message) {
        fprintf(stderr, "internal error: no message data with window's start signal\n");
        return 1;
    }
    FXElemBuildWindow(this->frame, message, ELEM_CREATE_NOW);
    return 1;
}

Plot CreateNewPlot() {
    Plot plot;
    const double x0 = 0.0001, x1 = 8 * math::Tau();
    plot.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);
    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    return plot;
}

void WorkerThreadStart(FXGUISignal *start_signal) {
    InitializeFonts();
    utils::Sleep(3);
    Window win(new elp_window_fox(start_signal));
    Plot plot = CreateNewPlot();
    win.Attach(plot, "");
    win.Start(640, 480, WindowResize);
    win.Wait();
}

int main(int argc, char *argv[]) {
    FXApp app("Plot Windows", "libelplot");
    app.init(argc, argv);
    auto main_window = new PlotWindow(&app, "FOX Window host example", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    std::thread worker_thread(WorkerThreadStart, main_window->start_signal);
    app.create();
    main_window->show(PLACEMENT_SCREEN);
    app.run();
    worker_thread.join();
    return 0;
}
