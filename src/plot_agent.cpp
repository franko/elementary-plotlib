#include "plot_agent.h"

namespace libcanvas {

void PlotAgent::AddWindow(Window *w, int slot_index) {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        if (linked_windows_[i].window == w) {
            linked_windows_[i].slot_index = slot_index;
            return;
        }
    }
    linked_windows_.add(WindowIndexPair{w, slot_index});
}

void PlotAgent::UpdateWindows() {
    for (unsigned i = 0; i < linked_windows_.size(); i++) {
        WindowIndexPair& pair = linked_windows_[i];
        pair.window->SlotRefresh(pair.slot_index);
    }
}

void PlotAgent::Clear() {
    linked_windows_.clear();
}
}
