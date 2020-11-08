#include "sdl_elem_window_factory.h"
#include "window_platform_sdl.h"

elem_window *sdl_elem_window_factory::create() {
    return new elem_window_sdl{};
}

elem_window *sdl_elem_window_factory::create_with_layout(const char *layout) {
    return new elem_window_sdl{layout};
}
