#pragma once

#include "agg_array.h"

class elem_window;

namespace graphics {

class window_surface;

class plot_agent {
    struct window_index_pair {
        elem_window *window;
        // FIXME: check if we can actually remove the pointer below.
        window_surface *surface;
        int slot_index;
    };
public:
    plot_agent() { }
    plot_agent(const plot_agent& other):
        linked_windows_(other.linked_windows_) {
    }
    plot_agent(plot_agent&& other):
        linked_windows_(other.linked_windows_) {
        other.linked_windows_.clear();
    }
    void add_window(elem_window *window, int slot_index);
    void update_windows();
    void clear();
    void clear_pending_flags();
    agg::pod_bvector<elem_window *> linked_windows() const;
private:
    agg::pod_bvector<window_index_pair> linked_windows_;
};
}
