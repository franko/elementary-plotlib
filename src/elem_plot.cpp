#include "elem/elem_c.h"
#include "elem_plot.h"
#include "elem_window.h"
#include "gc_context.h"
#include "global_elem_window_factory.h"

elem_plot::~elem_plot() {
    debug_log(2, "elem_plot::~elem_plot() [%p]", this);
}

void elem_plot::release() {
    std::lock_guard<std::mutex> guard(global_window_gc_mutex);
    m_ref_count--;
    gc_context gc;
    if (!has_references(gc)) {
        gc.collect_visited();
    }
}

bool elem_plot::has_references(gc_context& gc) {
    if (gc.visited(this)) {
        return false;
    }
    gc.add_visited(this);
    if (m_ref_count > 0) {
        return true;
    }
    auto linked_windows_list = m_plot_agent.linked_windows();
    for (unsigned i = 0; i < linked_windows_list.size(); i++) {
        auto linked_window = linked_windows_list[i];
        if (linked_window->has_references(gc)) {
            return true;
        }
    }
    return false;
}

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
