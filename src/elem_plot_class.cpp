#include "elem_plot_class.h"
#include "window_surface.h"

bool elem_plot::release() {
    m_ref_count--;
    gc_context gc;
    return has_references(gc);
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
