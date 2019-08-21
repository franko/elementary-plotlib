#pragma once

#include "agg_array.h"

namespace graphics {

class window;

class plot_agent {
    struct window_index_pair {
        window *window;
        int slot_index;
    };
public:
    void add_window(window *w, int slot_index);
    void update_windows();
private:
    agg::pod_bvector<window_index_pair> m_linked_windows;
};
}
