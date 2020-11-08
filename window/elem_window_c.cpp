// public declarations header.
#include "elem/elem_c.h"

// the following are private headers.
#include "global_elem_window_factory.h"
#include "plot.h"
#include "plot_agent.h"
#include "elem_window.h"
#include "elem/elem_c_forward.h"

void elem_plot_show(elem_plot *plot, unsigned width, unsigned height, unsigned flags) {
    plot->show(width, height, flags);
}

void elem_plot_wait(elem_plot *plot) {
    plot->wait_show_window();
}

elem_window *elem_window_new() {
    return global_elem_window_factory->create();
}

elem_window *elem_window_new_with_layout(const char *fmt) {
    return global_elem_window_factory->create_with_layout(fmt);
}

void elem_window_set_layout(elem_window *win, const char *fmt) {
    win->set_layout(fmt);
}

int elem_window_attach(elem_window *win, elem_plot *plot, const char* slot_str) {
    int index = win->attach(plot, slot_str);
    if (index >= 0) {
        plot->add_window_link(win, index);
    }
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
    win->release();
}
