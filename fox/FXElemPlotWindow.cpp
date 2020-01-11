#include "FXElemPlotWindow.h"
#include "window_fox_kernel.h"

FXDEFMAP(FXElemPlotWindow) FXElemPlotWindowMap[] = {
    FXMAPFUNC(SEL_MAP, 0, FXElemPlotWindow::onMap),
    FXMAPFUNC(SEL_PAINT, 0, FXElemPlotWindow::onPaint),
    FXMAPFUNC(SEL_IO_READ, FXElemPlotWindow::ID_UPDATE_REGION, FXElemPlotWindow::onUpdateRegion),
};

FXIMPLEMENT(FXElemPlotWindow,FXWindow,FXElemPlotWindowMap,ARRAYNUMBER(FXElemPlotWindowMap))

FXElemPlotWindow::FXElemPlotWindow(FXComposite* p, window_fox_kernel *win, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXWindow(p, opts, x, y, w, h), m_window(win)
{
    flags |= FLAG_SHOWN;
}

FXElemPlotWindow::~FXElemPlotWindow() {
}

void FXElemPlotWindow::position(FXint x, FXint y, FXint w, FXint h) {
    if (m_window) {
        m_window->on_resize(w, h);
    }
    FXWindow::position(x, y, w, h);
}

void FXElemPlotWindow::create() {
    if (m_window) {
        m_window->set_status(graphics::window_starting);
    }
    FXWindow::create();
}

long FXElemPlotWindow::onUpdateRegion(FXObject *, FXSelector, void *) {
    if (m_window) {
        m_window->call_update_region();
    }
    return 1;
}

long FXElemPlotWindow::onPaint(FXObject *sender, FXSelector sel, void *ptr) {
    if (m_window) {
        FXEvent *ev=(FXEvent*)ptr;
        m_window->draw(ev);
    }
    // TODO: should we do some blank drawing if no window is associated ?
    return 1;
}

long FXElemPlotWindow::onMap(FXObject *, FXSelector, void *) {
    if (m_window) {
        m_window->set_thread_id();
        m_window->set_status(graphics::window_running);
    }
    return 1;
}
