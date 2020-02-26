#pragma once

#include "agg_array.h"

namespace graphics {

class window_surface;

class plot_agent {
    struct window_index_pair {
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
    void add_window(window_surface *surface, int slot_index);
    void update_windows();
    void clear();
    void clear_pending_flags();
    agg::pod_bvector<window_surface*> linked_windows() const;
private:
    agg::pod_bvector<window_index_pair> linked_windows_;
};
}
