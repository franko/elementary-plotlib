#include "fox/GraphicsWindow.h"
#include "fox/window_fox.h"
#include "window_flags.h"
#include "debug_priv.h"

#include "rendering_buffer_utils.h"

namespace libcanvas {

FXDEFMAP(GraphicsWindow) GraphicsWindowMap[] = {
    FXMAPFUNC(SEL_MAP,     0,                            GraphicsWindow::onMap),
    FXMAPFUNC(SEL_PAINT,   0,                            GraphicsWindow::onPaint),
    FXMAPFUNC(SEL_IO_READ, GraphicsWindow::ID_UPDATE_REGION, GraphicsWindow::onUpdateRegion),
};

FXIMPLEMENT(GraphicsWindow,FXWindow,GraphicsWindowMap,ARRAYNUMBER(GraphicsWindowMap))

GraphicsWindow::GraphicsWindow(FXComposite* p, const char *split_str, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXWindow(p, opts, x, y, w, h), m_window_impl((WindowImpl *) new window_fox(this, split_str))
{
    flags |= FLAG_SHOWN;
}

int GraphicsWindow::Attach(libcanvas::Plot& p, const char* slot_str) {
    window_fox *window_impl = (window_fox *) m_window_impl;
    graphics::plot *plot_impl = (graphics::plot *) p.plot_impl_;
    return window_impl->attach(plot_impl, slot_str);
}

void GraphicsWindow::SlotRefresh(unsigned index) {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->slot_refresh(index);
}

void GraphicsWindow::Wait() {
    window_fox *window_impl = (window_fox *) m_window_impl;
    notify_request<graphics::window_status_e> req{graphics::window_closed};
    int retval = window_impl->set_notify_request(req);
    if (retval == request_success) {
        req.wait();
    }
}

GraphicsWindow::~GraphicsWindow() {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->set_window_status(graphics::window_closed);
}

void GraphicsWindow::position(FXint x, FXint y, FXint w, FXint h) {
    debug_log(1, "GraphicsWindow::position: %d %d", w, h);
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->on_resize(w, h);
    FXWindow::position(x, y, w, h);
}

void GraphicsWindow::create() {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->set_window_status(graphics::window_starting);
    FXWindow::create();
}

long GraphicsWindow::onUpdateRegion(FXObject *, FXSelector, void *) {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->call_update_region();
    return 1;
}

long GraphicsWindow::onPaint(FXObject *, FXSelector, void *ptr) {
    FXEvent *ev=(FXEvent*)ptr;
    debug_log(1, "paint event");
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->draw(ev);
    return 1;
}

long GraphicsWindow::onMap(FXObject *, FXSelector, void *) {
    debug_log(1, "GraphicsWindow: map event");
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->set_thread_id();
    window_impl->set_window_status(graphics::window_running);
    return 1;
}
}
