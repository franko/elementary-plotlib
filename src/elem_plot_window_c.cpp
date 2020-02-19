// public declarations header.
#include "elem_plot_c.h"

// the following are private headers.
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "elem_plot_c_forward.h"
#include "window_platform_native.h"

elem_window *elem_window_new() {
    return new elem_window_native{};
}

elem_window *elem_window_new_with_layout(const char *fmt) {
    return new elem_window_native{fmt};;
}

void elem_window_set_layout(elem_window *win, const char *fmt) {
    win->set_layout(fmt);
}

int elem_window_attach(elem_window *win, elem_plot *plot, const char* slot_str) {
    graphics::plot *p = plot->plot;
    int index = win->attach(p, slot_str);
    graphics::plot_agent *agent = (graphics::plot_agent *) plot->plot_agent;
    agent->add_window(win->get_window_surface(), index);
    return index;
}

void elem_window_slot_refresh(elem_window *win, unsigned index) {
    win->slot_refresh(index);
}

void elem_window_start(elem_window *win, unsigned width, unsigned height, unsigned flags) {
    win->start(width, height, flags);
}

void elem_window_wait(elem_window *win) {
    win->wait();
}

void elem_window_free(elem_window *win) {
    delete win;
}
