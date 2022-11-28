#include "elem_plot.h"
#include "elem_window.h"
#include "gc_context.h"

graphics::plot *elem_plot_manager_ref::copy(const graphics::plot *p) {
    // We voulountarily discard the const qualifier. In reality it holds for the
    // graphics::plot data payload but not for its extension belonging to elem_plot.
    auto ep = elem_plot::assert_cast((graphics::plot *) p);
    ep->retain();
    return (graphics::plot *) p;
}

void elem_plot_manager_ref::dispose(graphics::plot *p) {
    auto ep = elem_plot::assert_cast(p);
    ep->release();
}

elem_plot_manager_ref elem_plot::legends_plot_manager[1];

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

void elem_plot::retain() {
    std::lock_guard<std::mutex> guard(global_window_gc_mutex);
    m_ref_count++;
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
