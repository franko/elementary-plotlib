// public declarations header.
#include "libcanvas.h"

// the following are private headers.
#include "plot.h"
#include "window.h"

#ifdef WIN32
typedef window_gen<window_win32> PlatformWindowImpl;
#else
typedef window_gen<xwindow> PlatformWindowImpl;
#endif

namespace libcanvas {

Window::Window() : window_impl_{(WindowImpl *) new PlatformWindowImpl()} {
}

Window::Window(const char *fmt) : window_impl_{(WindowImpl *) new PlatformWindowImpl(fmt)} {
}

Window::~Window() {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    delete win;
}

int Window::Attach(Plot& plot, const char* slot_str) {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    graphics::plot *p = (graphics::plot *) plot.plot_impl_;
    return win->attach(p, slot_str);
}

void Window::SlotRefresh(unsigned index) {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    win->slot_refresh(index);
}

void Window::Start(unsigned width, unsigned height, unsigned flags) {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    win->start(width, height, flags);
}

void Window::Wait() {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    win->wait();
}
}
