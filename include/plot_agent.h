#pragma once

#include "agg_array.h"

namespace graphics {

class plot_agent {
    struct window_index_pair {
        void *window;
        int slot_index;
    };
public:
    void add_window(void *w, int slot_index);
    void update_windows();
private:
    agg::pod_bvector<window_index_pair> m_linked_windows;
};
}
