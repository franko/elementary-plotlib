#include "fox_elem_window_factory.h"
#include "window_platform_fox.h"

elem_window *fox_elem_window_factory::create() {
    return new elem_window_fox{m_start_signal};
}

elem_window *fox_elem_window_factory::create_with_layout(const char *layout) {
    return new elem_window_fox{layout, m_start_signal};
}
