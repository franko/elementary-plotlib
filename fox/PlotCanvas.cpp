#include "fox/PlotCanvas.h"
#include "fox/window_fox.h"
#include "window_flags.h"
#include "debug_log.h"

#include "rendering_buffer_utils.h"

FXDEFMAP(PlotCanvas) PlotCanvasMap[] = {
    FXMAPFUNC(SEL_PAINT,   0,                            PlotCanvas::onPaint),
    FXMAPFUNC(SEL_IO_READ, PlotCanvas::ID_UPDATE_REGION, PlotCanvas::onUpdateRegion),
    FXMAPFUNC(SEL_MAP,     0,                            PlotCanvas::onMap),
};

FXIMPLEMENT(PlotCanvas,FXWindow,PlotCanvasMap,ARRAYNUMBER(PlotCanvasMap))

void PlotCanvas::position(FXint x, FXint y, FXint w, FXint h) {
    debug_log("PlotCanvas::position: %d %d", w, h);
    m_window->on_resize(w, h);
    FXWindow::position(x, y, w, h);
}

void PlotCanvas::create() {
    m_window->set_window_status(graphics::window_starting);
    FXWindow::create();
}

void PlotCanvas::destroy() {
    debug_log("PlotCanvas: destroy event");
    m_window->set_window_status(graphics::window_closed);
    FXWindow::destroy();
}

long PlotCanvas::onUpdateRegion(FXObject *, FXSelector, void *) {
    m_window->call_update_region();
    return 1;
}

long PlotCanvas::onPaint(FXObject *, FXSelector, void *ptr) {
    FXEvent *ev=(FXEvent*)ptr;
    debug_log("paint event");
    m_window->draw(ev);
    return 1;
}

long PlotCanvas::onMap(FXObject *, FXSelector, void *) {
    debug_log("PlotCanvas: map event");
    m_window->set_window_status(graphics::window_running);
    return 1;
}
