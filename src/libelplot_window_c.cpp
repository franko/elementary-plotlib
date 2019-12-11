// public declarations header.
#include "libelplot_c.h"

// the following are private headers.
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "libelplot_c_forward.h"

elp_window *elp_window_new() {
    return (elp_window *) new graphics::window{};
}

elp_window *elp_window_new_with_layout(const char *fmt) {
    return (elp_window *) new graphics::window{fmt};
}

void elp_window_set_layout(elp_window *win_, const char *fmt) {
    graphics::window *win = (graphics::window *) win_;
    win->set_layout(fmt);
}

int elp_window_attach(elp_window *win_, elp_plot *plot, const char* slot_str) {
    graphics::window *win = (graphics::window *) win_;
    graphics::plot *p = plot->plot;
    int index = win->attach(p, slot_str);
    graphics::plot_agent *agent = (graphics::plot_agent *) plot->plot_agent;
    agent->add_window(win->window_surface(), index);
    return index;
}

void elp_window_slot_refresh(elp_window *win_, unsigned index) {
    graphics::window *win = (graphics::window *) win_;
    win->slot_refresh(index);
}

void elp_window_start(elp_window *win_, unsigned width, unsigned height, unsigned flags) {
    graphics::window *win = (graphics::window *) win_;
    win->start(width, height, flags);
}

void elp_window_wait(elp_window *win_) {
    graphics::window *win = (graphics::window *) win_;
    win->wait();
}

void elp_window_free(elp_window *win_) {
    graphics::window *win = (graphics::window *) win_;
    delete win;
}
