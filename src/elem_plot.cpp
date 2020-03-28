#include "elem_plot.h"
#include "elem_window.h"
#include "gc_context.h"

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
