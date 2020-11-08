#include "global_elem_window_factory.h"
#include "native_elem_window_factory.h"
#include "sdl_elem_window_factory.h"

// elem_window_factory *global_elem_window_factory = new native_elem_window_factory{};
elem_window_factory *global_elem_window_factory = new sdl_elem_window_factory{};
