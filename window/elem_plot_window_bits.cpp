#include "elem/elem_c.h"
#include "elem_plot.h"
#include "elem_window.h"
#include "global_elem_window_factory.h"

void elem_plot::show(unsigned width, unsigned height, unsigned flags) {
    // Ignore a possible not-null m_show_window, it will be freed by the GC
    // when the plot will go away.
    // The only caveat is that when plot::wait() is called we will
    // wait only of the last generated window.
    m_show_window = global_elem_window_factory->create();
    elem_window_attach(m_show_window, this, "");
    m_show_window->start(width, height, flags);
    // We release here the window because it is already linked to the plot
    // with the attach() operation. It means it will not be freed by the GC
    // as long as the plot itself is not freed.
    m_show_window->release();
}

void elem_plot::wait_show_window() {
    if (m_show_window) {
        m_show_window->wait();
    }
}
