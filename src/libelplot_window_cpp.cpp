// public declarations header.
#include "libelplot.h"

// the following are private headers.
#include "plot.h"
#include "plot_agent.h"
#include "window_platform_native.h"

namespace elp {

Window::Window() : window_impl_(new elp_window_native()) {
}

Window::Window(const char *layout) : window_impl_{new elp_window_native(layout)} {
}

Window::~Window() {
    delete window_impl_;
}

void Window::SetLayout(const char *fmt) {
    window_impl_->set_layout(fmt);    
}

int Window::Attach(Plot& plot, const char* slot_str) {
    graphics::plot *p = plot.plot_impl_.plot;
    int index = window_impl_->attach(p, slot_str);
    graphics::plot_agent *agent = plot.plot_impl_.plot_agent;
    agent->add_window(window_impl_->get_window_surface(), index);
    return index;
}

void Window::SlotRefresh(unsigned index) {
    window_impl_->slot_refresh(index);
}

void Window::Start(unsigned width, unsigned height, unsigned flags) {
    window_impl_->start(width, height, flags);
}

void Window::Wait() {
    window_impl_->wait();
}
}
