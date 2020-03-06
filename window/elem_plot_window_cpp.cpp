// public declarations header.
#include "elem_plot.h"

// the following are private headers.
#include "elem_plot_class.h"
#include "elem_window.h"
#include "global_elem_window_factory.h"
#include "plot.h"
#include "plot_agent.h"

namespace elem {

Window::Window() : window_impl_(global_elem_window_factory->create()) {
}

Window::Window(const char *layout) : window_impl_{global_elem_window_factory->create_with_layout(layout)} {
}

Window::Window(elem_window *window_impl_) : window_impl_(window_impl_) {
}

Window::~Window() {
    window_impl_->release();
}

void Window::SetLayout(const char *fmt) {
    window_impl_->set_layout(fmt);
}

int Window::Attach(Plot& plot, const char* slot_str) {
    int index = window_impl_->attach(plot.plot_impl_, slot_str);
    plot.plot_impl_->add_window_link(window_impl_, index);
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

void Window::Close() {
    window_impl_->close();
}
}
