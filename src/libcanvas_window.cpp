// public declarations header.
#include "libcanvas.h"

// the following are private headers.
#include "plot.h"
#include "plot_agent.h"
#include "window.h"

namespace libcanvas {

Window::Window() : window_impl_{(WindowImpl *) new graphics::window()} {
}

Window::Window(const char *fmt) : window_impl_{(WindowImpl *) new graphics::window(fmt)} {
}

Window::~Window() {
    graphics::window *win = (graphics::window *) window_impl_;
    delete win;
}

Window& Window::Attach(Plot& plot, const char* slot_str, int *slot_index) {
    graphics::window *win = (graphics::window *) window_impl_;
    graphics::plot *p = (graphics::plot *) plot.plot_impl_;
    int index = win->attach(p, slot_str);
    graphics::plot_agent *agent = (graphics::plot_agent *) plot.plot_agent_impl_;
    agent->add_window(win->window_surface(), index);
    if (slot_index) {
        *slot_index = index;
    }
    return *this;
}

Window& Window::SlotRefresh(unsigned index) {
    graphics::window *win = (graphics::window *) window_impl_;
    win->slot_refresh(index);
    return *this;
}

Window& Window::Start(unsigned width, unsigned height, unsigned flags) {
    graphics::window *win = (graphics::window *) window_impl_;
    win->start(width, height, flags);
    return *this;
}

Window& Window::Wait() {
    graphics::window *win = (graphics::window *) window_impl_;
    win->wait();
    return *this;
}
}
