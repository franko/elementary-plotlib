// public declarations header.
#include "libelplot_c.h"

// the following are private headers.
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "libelplot_c_forward.h"
#include "window_platform_native.h"

elp_window *elp_window_new() {
    return new elp_window_native{};
}

elp_window *elp_window_new_with_layout(const char *fmt) {
    return new elp_window_native{fmt};;
}

void elp_window_set_layout(elp_window *win, const char *fmt) {
    win->set_layout(fmt);
}

int elp_window_attach(elp_window *win, elp_plot *plot, const char* slot_str) {
    graphics::plot *p = plot->plot;
    int index = win->attach(p, slot_str);
    graphics::plot_agent *agent = (graphics::plot_agent *) plot->plot_agent;
    agent->add_window(win->get_window_surface(), index);
    return index;
}

void elp_window_slot_refresh(elp_window *win, unsigned index) {
    win->slot_refresh(index);
}

void elp_window_start(elp_window *win, unsigned width, unsigned height, unsigned flags) {
    win->start(width, height, flags);
}

void elp_window_wait(elp_window *win) {
    win->wait();
}

void elp_window_free(elp_window *win) {
    delete win;
}
