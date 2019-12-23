#include "FXElpWindow.h"
#include "window_fox.h"
#include "window_flags.h"
#include "plot_agent.h"
#include "debug_priv.h"

#include "rendering_buffer_utils.h"

FXDEFMAP(FXElpWindow) GraphicsWindowMap[] = {
    FXMAPFUNC(SEL_MAP,     0,                            FXElpWindow::onMap),
    FXMAPFUNC(SEL_PAINT,   0,                            FXElpWindow::onPaint),
    FXMAPFUNC(SEL_IO_READ, FXElpWindow::ID_UPDATE_REGION, FXElpWindow::onUpdateRegion),
};

FXIMPLEMENT(FXElpWindow,FXWindow,GraphicsWindowMap,ARRAYNUMBER(GraphicsWindowMap))

FXElpWindow::FXElpWindow(FXComposite* p, const char *split_str, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXWindow(p, opts, x, y, w, h), m_window_impl(new window_fox(this, split_str))
{
    flags |= FLAG_SHOWN;
}

int FXElpWindow::Attach(elp::Plot& p, const char* slot_str) {
    graphics::plot *plot_impl = p.plot_impl_.plot;
    int index = m_window_impl->attach(plot_impl, slot_str);
    graphics::plot_agent *agent = p.plot_impl_.plot_agent;
    agent->add_window(m_window_impl->get_window_surface(), index);
    return index;
}

void FXElpWindow::SetLayout(const char *fmt) {
    if (m_window_impl->status() == graphics::window_not_started) {
        m_window_impl->get_window_surface()->split(fmt);
    }
}

void FXElpWindow::SlotRefresh(unsigned index) {
    m_window_impl->slot_refresh(index);
}

void FXElpWindow::Wait() {
    notify_request<graphics::window_status_e> req{graphics::window_closed};
    int retval = m_window_impl->set_notify_request(req);
    if (retval == request_success) {
        req.wait();
    }
}

FXElpWindow::~FXElpWindow() {
    m_window_impl->set_status(graphics::window_closed);
}

void FXElpWindow::position(FXint x, FXint y, FXint w, FXint h) {
    debug_log(1, "FXElpWindow::position: %d %d", w, h);
    m_window_impl->on_resize(w, h);
    FXWindow::position(x, y, w, h);
}

void FXElpWindow::create() {
    m_window_impl->set_status(graphics::window_starting);
    FXWindow::create();
}

long FXElpWindow::onUpdateRegion(FXObject *, FXSelector, void *) {
    m_window_impl->call_update_region();
    return 1;
}

long FXElpWindow::onPaint(FXObject *, FXSelector, void *ptr) {
    FXEvent *ev=(FXEvent*)ptr;
    debug_log(1, "paint event");
    m_window_impl->draw(ev);
    return 1;
}

long FXElpWindow::onMap(FXObject *, FXSelector, void *) {
    debug_log(1, "FXElpWindow: map event");
    m_window_impl->set_thread_id();
    m_window_impl->set_status(graphics::window_running);
    return 1;
}
