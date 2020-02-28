#include <cstdio>

#include "gc_context.h"
#include "elem_plot_class.h"
#include "window.h"

std::mutex global_window_gc_mutex;

void gc_context::collect_visited() {
    for (unsigned i = 0; i < m_visited_windows.size(); i++) {
        fprintf(stderr, "collect window: %p\n", m_visited_windows[i]); fflush(stderr);
        delete m_visited_windows[i];
    }
    for (unsigned i = 0; i < m_visited_plots.size(); i++) {
        fprintf(stderr, "collect plot: %p\n", m_visited_plots[i]); fflush(stderr);
        delete m_visited_plots[i];
    }
}
