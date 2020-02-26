#pragma once

#include <agg_array.h>

namespace graphics {
    class plot;
    class window_surface;
}

class gc_context {
    using plot_type = graphics::plot;
    using window_type = graphics::window_surface;
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
private:
    agg::pod_bvector<plot_type*> m_visited_plots;
    agg::pod_bvector<window_type*> m_visited_windows;
};
