local ffi = require("ffi")

ffi.cdef[[
enum {
    elp_window_resize            = 1,
    elp_window_hw_buffer         = 2,
    elp_window_keep_aspect_ratio = 4,
    elp_window_process_all_keys  = 8,
};

elp_window *elp_window_new();
elp_window *elp_window_new_with_layout(const char *fmt);
void elp_window_set_layout(elp_window *win, const char *fmt);
void elp_window_free(elp_window *w);
int elp_window_attach(elp_window *win, elp_plot *plot, const char* slot_str);
void elp_window_slot_refresh(elp_window *win, unsigned index);
void elp_window_start(elp_window *win, unsigned width, unsigned height, unsigned flags);
void elp_window_wait(elp_window *win);
]]

return ffi.load('libelplotwin')
