#include "plot_agent.h"

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
        pair.surface->slot_refresh(pair.slot_index);
    }
}

void plot_agent::clear() {
    linked_windows_.clear();
}
}
