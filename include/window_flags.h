#ifndef WINDOW_FLAGS_H
#define WINDOW_FLAGS_H

namespace graphics {

//----------------------------------------------------------window_flag_e
// These are flags used in method init(). Not all of them are
// applicable on different platforms, for example the win32_api
// cannot use a hardware buffer (window_hw_buffer).
// The implementation should simply ignore unsupported flags.
enum window_flag_e
{
    window_resize            = 1,
    window_hw_buffer         = 2,
    window_keep_aspect_ratio = 4,
    window_process_all_keys  = 8,
};

enum {
    window_min_width  = 128,
    window_min_height = 128,
    window_max_width  = 4096,
    window_max_height = 4096,
};

enum {
    window_not_started,
    window_starting,
    window_running,
    window_closed
};

}

#endif
