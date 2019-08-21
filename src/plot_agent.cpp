#include "plot_agent.h"
#include "window.h"

namespace graphics {

void plot_agent::add_window(void *w, int slot_index) {
    for (unsigned i = 0; i < m_linked_windows.size(); i++) {
        if (m_linked_windows[i].window == w) {
            m_linked_windows[i].slot_index = slot_index;
            return;
        }
    }
    m_linked_windows.add(window_index_pair{w, slot_index});
}

void plot_agent::update_windows() {
    for (unsigned i = 0; i < m_linked_windows.size(); i++) {
        window_index_pair& pair = m_linked_windows[i];
        window *w = (graphics::window *) pair.window;
        w->slot_refresh(pair.slot_index);
    }
}
}