#pragma once

#include "agg_array.h"

#include "libcanvas.h"

namespace graphics {
class plot_agent {
    struct window_index_pair {
        libcanvas::Window *window;
        int slot_index;
    };
public:
    void add_window(libcanvas::Window *w, int slot_index);
    void update_windows();
private:
    agg::pod_bvector<window_index_pair> m_linked_windows;
};
}
