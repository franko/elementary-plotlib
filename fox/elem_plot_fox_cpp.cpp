#include "elem_plot_fox_cpp.h"
#include "fox_elem_window_factory.h"
#include "global_elem_window_factory.h"

void elem::SetFoxWindowSystem(FXGUISignal *start_signal) {
    elem_window_factory *new_window_factory = new fox_elem_window_factory{start_signal};
    delete global_elem_window_factory;
    global_elem_window_factory = new_window_factory;
}
