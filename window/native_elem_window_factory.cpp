#include "native_elem_window_factory.h"
#include "window_platform_native.h"

elem_window *native_elem_window_factory::create() {
    return new elem_window_native{};
}

elem_window *native_elem_window_factory::create_with_layout(const char *layout) {
    return new elem_window_native{layout};
}
