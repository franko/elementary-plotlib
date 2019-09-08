#include "fox/LibcanvasWindow.h"
#include "fox/window_fox.h"
#include "window_flags.h"
#include "plot_agent.h"
#include "debug_priv.h"

#include "rendering_buffer_utils.h"

FXDEFMAP(LibcanvasWindow) GraphicsWindowMap[] = {
    FXMAPFUNC(SEL_MAP,     0,                            LibcanvasWindow::onMap),
    FXMAPFUNC(SEL_PAINT,   0,                            LibcanvasWindow::onPaint),
    FXMAPFUNC(SEL_IO_READ, LibcanvasWindow::ID_UPDATE_REGION, LibcanvasWindow::onUpdateRegion),
};

FXIMPLEMENT(LibcanvasWindow,FXWindow,GraphicsWindowMap,ARRAYNUMBER(GraphicsWindowMap))

LibcanvasWindow::LibcanvasWindow(FXComposite* p, const char *split_str, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXWindow(p, opts, x, y, w, h), m_window_impl((WindowImpl *) new window_fox(this, split_str))
{
    flags |= FLAG_SHOWN;
}

int LibcanvasWindow::Attach(libcanvas::Plot& p, const char* slot_str) {
    window_fox *window_impl = (window_fox *) m_window_impl;
    graphics::plot *plot_impl = (graphics::plot *) p.plot_impl_;
    int index = window_impl->attach(plot_impl, slot_str);
    graphics::plot_agent *agent = (graphics::plot_agent *) p.plot_agent_impl_;
    agent->add_window(window_impl->window_surface(), index);
    return index;
}

void LibcanvasWindow::SlotRefresh(unsigned index) {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->slot_refresh(index);
}

void LibcanvasWindow::Wait() {
    window_fox *window_impl = (window_fox *) m_window_impl;
    notify_request<graphics::window_status_e> req{graphics::window_closed};
    int retval = window_impl->set_notify_request(req);
    if (retval == request_success) {
        req.wait();
    }
}

LibcanvasWindow::~LibcanvasWindow() {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->set_window_status(graphics::window_closed);
}

void LibcanvasWindow::position(FXint x, FXint y, FXint w, FXint h) {
    debug_log(1, "LibcanvasWindow::position: %d %d", w, h);
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->on_resize(w, h);
    FXWindow::position(x, y, w, h);
}

void LibcanvasWindow::create() {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->set_window_status(graphics::window_starting);
    FXWindow::create();
}

long LibcanvasWindow::onUpdateRegion(FXObject *, FXSelector, void *) {
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->call_update_region();
    return 1;
}

long LibcanvasWindow::onPaint(FXObject *, FXSelector, void *ptr) {
    FXEvent *ev=(FXEvent*)ptr;
    debug_log(1, "paint event");
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->draw(ev);
    return 1;
}

long LibcanvasWindow::onMap(FXObject *, FXSelector, void *) {
    debug_log(1, "LibcanvasWindow: map event");
    window_fox *window_impl = (window_fox *) m_window_impl;
    window_impl->set_thread_id();
    window_impl->set_window_status(graphics::window_running);
    return 1;
}
