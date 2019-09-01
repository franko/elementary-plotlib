#pragma once

#include "agg_array.h"
#include "window_surface.h"

namespace graphics {

class plot_agent {
    struct window_index_pair {
        window_surface *surface;
        int slot_index;
    };
public:
    void add_window(window_surface *surface, int slot_index);
    void update_windows();
    void clear();
private:
    agg::pod_bvector<window_index_pair> linked_windows_;
};
}
