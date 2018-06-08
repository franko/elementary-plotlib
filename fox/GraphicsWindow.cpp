#include "fox/GraphicsWindow.h"
#include "window_flags.h"
#include "debug_log.h"

#include "rendering_buffer_utils.h"

FXDEFMAP(GraphicsWindow) GraphicsWindowMap[] = {
    FXMAPFUNC(SEL_MAP,     0,                            GraphicsWindow::onMap),
    FXMAPFUNC(SEL_PAINT,   0,                            GraphicsWindow::onPaint),
    FXMAPFUNC(SEL_IO_READ, GraphicsWindow::ID_UPDATE_REGION, GraphicsWindow::onUpdateRegion),
};

FXIMPLEMENT(GraphicsWindow,FXWindow,GraphicsWindowMap,ARRAYNUMBER(GraphicsWindowMap))

GraphicsWindow::GraphicsWindow(FXComposite* p, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXWindow(p, opts, x, y, w, h), m_surface(nullptr), m_window(m_surface, this)
{
    flags |= FLAG_SHOWN;
    m_surface.attach_window(&m_window);
}

GraphicsWindow::~GraphicsWindow() {
    m_window.set_window_status(graphics::window_closed);
}

void GraphicsWindow::position(FXint x, FXint y, FXint w, FXint h) {
    debug_log("GraphicsWindow::position: %d %d", w, h);
    m_window.on_resize(w, h);
    FXWindow::position(x, y, w, h);
}

void GraphicsWindow::create() {
    m_window.set_window_status(graphics::window_starting);
    FXWindow::create();
}

long GraphicsWindow::onUpdateRegion(FXObject *, FXSelector, void *) {
    m_window.call_update_region();
    return 1;
}

long GraphicsWindow::onPaint(FXObject *, FXSelector, void *ptr) {
    FXEvent *ev=(FXEvent*)ptr;
    debug_log("paint event");
    m_window.draw(ev);
    return 1;
}

long GraphicsWindow::onMap(FXObject *, FXSelector, void *) {
    debug_log("GraphicsWindow: map event");
    m_window.set_thread_id();
    m_window.set_window_status(graphics::window_running);
    return 1;
}
