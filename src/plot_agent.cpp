#include "plot_agent.h"
#include "window_surface.h"
#include "window.h"

namespace graphics {

void plot_agent::add_window(elem_window *window, int slot_index) {
    window_surface *surface = window->get_window_surface();
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        if (linked_windows_[i].surface == surface) {
            linked_windows_[i].slot_index = slot_index;
            return;
        }
    }
    linked_windows_.add(window_index_pair{window, surface, slot_index});
}

void plot_agent::update_windows() {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        window_index_pair& pair = linked_windows_[i];
        // Note: do not use a window_surface for plot_agent
        // but an opaque structure proxy.
        pair.surface->slot_refresh_request(pair.slot_index);
    }
}

void plot_agent::clear() {
    linked_windows_.clear();
}

void plot_agent::clear_pending_flags() {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        window_index_pair& pair = linked_windows_[i];
        pair.surface->clear_pending_flags(pair.slot_index);
    }
}

agg::pod_bvector<elem_window *> plot_agent::linked_windows() const {
    agg::pod_bvector<elem_window *> windows_list;
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        windows_list.add(linked_windows_[i].window);
    }
    return windows_list;
}
}
