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

void Window::SetLayout(const char *fmt) {
    graphics::window *win = (graphics::window *) window_impl_;
    win->set_layout(fmt);    
}

int Window::Attach(Plot& plot, const char* slot_str) {
    graphics::window *win = (graphics::window *) window_impl_;
    graphics::plot *p = plot.plot_impl_.plot;
    int index = win->attach(p, slot_str);
    graphics::plot_agent *agent = plot.plot_impl_.plot_agent;
    agent->add_window(win->window_surface(), index);
    return index;
}

void Window::SlotRefresh(unsigned index) {
    graphics::window *win = (graphics::window *) window_impl_;
    win->slot_refresh(index);
}

void Window::Start(unsigned width, unsigned height, unsigned flags) {
    graphics::window *win = (graphics::window *) window_impl_;
    win->start(width, height, flags);
}

void Window::Wait() {
    graphics::window *win = (graphics::window *) window_impl_;
    win->wait();
}
}
