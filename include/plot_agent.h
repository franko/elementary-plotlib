#pragma once

#include "agg_array.h"
#include "libcanvas.h"

namespace libcanvas {

class PlotAgent {
    struct WindowIndexPair {
        Window *window;
        int slot_index;
    };
public:
    void AddWindow(Window *w, int slot_index);
    void UpdateWindows();
    void Clear();
private:
    agg::pod_bvector<WindowIndexPair> linked_windows_;
};
}
