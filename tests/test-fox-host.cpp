#include <cmath>
#include <thread>

#include "elem/elem.h"
#include "elem/elem_utils.h"
#include "FXElemBaseWindow.h"
#include "window_platform_fox.h"

using namespace elem;

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
        m_start_signal = new FXGUISignal(a, this, ID_PLOT_WINDOW_START, nullptr);
    }

    ~PlotWindow() {
        delete m_start_signal;
    }

    long onElemWindowStart(FXObject *, FXSelector, void *);

    enum {
        ID_PLOT_WINDOW_START = FXApp::ID_LAST,
        ID_LAST,
    };

    FXGUISignal *start_signal() {
        return m_start_signal;
    }

private:
    FXGUISignal *m_start_signal;
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
    FXApp *app = getApp();
    FXuint main_window_options = (DECOR_TITLE|DECOR_MINIMIZE|DECOR_MAXIMIZE|DECOR_CLOSE|DECOR_BORDER|DECOR_SHRINKABLE|DECOR_MENU);
    if (message->flags & WindowResize) {
        main_window_options |= DECOR_STRETCHABLE;
    }
    auto main_win = new FXMainWindow(app, "Plot Window", nullptr, nullptr, main_window_options, 0, 0, message->width, message->height);

    auto elem_window = new FXElemBaseWindow(main_win, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    elem_window->setWidth(message->width);
    elem_window->setHeight(message->height);
    elem_window->activateElem(message, ELEM_CREATE_DEFER);
    main_win->create();
    main_win->show(PLACEMENT_SCREEN);
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
    Window win(new elem_window_fox(start_signal));
    Plot plot = CreateNewPlot();
    win.Attach(plot, "");
    win.Start(640, 480, WindowResize);
    win.Wait();
}

int main(int argc, char *argv[]) {
    FXApp app("Plot Windows", "elem_plot");
    app.init(argc, argv);
    auto main_window = new PlotWindow(&app, "FOX Window host example", nullptr, nullptr, DECOR_ALL, 0, 0, 320, 320);
    new FXLabel(main_window, "Window Plot demonstrator");

    std::thread worker_thread(WorkerThreadStart, main_window->start_signal());
    worker_thread.detach();

    app.create();
    main_window->show(PLACEMENT_SCREEN);
    return app.run();
}
