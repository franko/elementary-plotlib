#pragma once

#include <mutex>
#include <agg_array.h>

class elem_plot;
class elem_window;

extern std::mutex global_window_gc_mutex;

class gc_context {
    using plot_type = elem_plot;
    using window_type = elem_window;
public:
    bool visited(plot_type *plot) const {
        for (unsigned i = 0; i < m_visited_plots.size(); i++) {
            if (plot == m_visited_plots[i]) {
                return true;
            }
        }
        return false;
    }

    void add_visited(plot_type *plot) {
        for (unsigned i = 0; i < m_visited_plots.size(); i++) {
            if (plot == m_visited_plots[i]) {
                return;
            }
        }
        m_visited_plots.add(plot);
    }

    bool visited(window_type *window) const {
        for (unsigned i = 0; i < m_visited_windows.size(); i++) {
            if (window == m_visited_windows[i]) {
                return true;
            }
        }
        return false;
    }

    void add_visited(window_type *window) {
        for (unsigned i = 0; i < m_visited_windows.size(); i++) {
            if (window == m_visited_windows[i]) {
                return;
            }
        }
        m_visited_windows.add(window);
    }

    void collect_visited();
private:
    agg::pod_bvector<plot_type*> m_visited_plots;
    agg::pod_bvector<window_type*> m_visited_windows;
};
