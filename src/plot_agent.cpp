#include "plot_agent.h"
#include "window_surface.h"

namespace graphics {

void plot_agent::add_window(window_surface *surface, int slot_index) {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        if (linked_windows_[i].surface == surface) {
            linked_windows_[i].slot_index = slot_index;
            return;
        }
    }
    linked_windows_.add(window_index_pair{surface, slot_index});
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

agg::pod_bvector<window_surface*> plot_agent::linked_windows() const {
    agg::pod_bvector<window_surface*> windows_list;
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        windows_list.add(linked_windows_[i].surface);
    }
    return windows_list;
}
}
