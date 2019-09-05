// public declarations header.
#include "libcanvas.h"

// the following are private headers.
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "libcanvas_c_priv.h"

canvas_window *canvas_window_new() {
    return (canvas_window *) new graphics::window{};
}

canvas_window *canvas_window_new_with_layout(const char *fmt) {
    return (canvas_window *) new graphics::window{fmt};
}

int canvas_window_attach(canvas_window *win, canvas_plot *plot, const char* slot_str) {
    graphics::window *win = (graphics::window *) win;
    graphics::plot *p = plot->plot;
    int index = win->attach(p, slot_str);
    graphics::plot_agent *agent = (graphics::plot_agent *) plot->plot_agent;
    agent->add_window(win->window_surface(), index);
    return index;
}

void canvas_window_slot_refresh(canvas_window *win_, unsigned index) {
    graphics::window *win = (graphics::window *) win_;
    win->slot_refresh(index);
}

void canvas_window_start(canvas_window *win_, unsigned width, unsigned height, unsigned flags) {
    graphics::window *win = (graphics::window *) win_;
    win->start(width, height, flags);
}

void canvas_window_wait(canvas_window *win_) {
    graphics::window *win = (graphics::window *) win_;
    win->wait();
}

void canvas_window_free(canvas_window *win_) {
    graphics::window *win = (graphics::window *) win_;
    delete win;
}
