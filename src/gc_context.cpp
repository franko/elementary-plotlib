#include "gc_context.h"
#include "elem_plot.h"
#include "elem_window.h"

std::mutex global_window_gc_mutex;

void gc_context::collect_visited() {
    for (unsigned i = 0; i < m_visited_windows.size(); i++) {
        delete m_visited_windows[i];
    }
    for (unsigned i = 0; i < m_visited_plots.size(); i++) {
        delete m_visited_plots[i];
    }
}
