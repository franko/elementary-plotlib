#include "plot_agent.h"
#include "window_surface.h"
#include "elem_window.h"

namespace graphics {

void plot_agent::add_window(elem_window *window, int slot_index) {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        window_index_pair&  test_pair = linked_windows_[i];
        if (test_pair.window == window && test_pair.slot_index == slot_index) {
            return;
        }
    }
    linked_windows_.add(window_index_pair{window, slot_index});
}

void plot_agent::update_windows() {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        window_index_pair& pair = linked_windows_[i];
        pair.window->get_window_surface()->slot_refresh_request(pair.slot_index);
    }
}

void plot_agent::clear() {
    linked_windows_.clear();
}

void plot_agent::clear_pending_flags() {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        window_index_pair& pair = linked_windows_[i];
        pair.window->get_window_surface()->clear_pending_flags(pair.slot_index);
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
