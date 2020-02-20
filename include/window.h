#pragma once

namespace graphics {
class plot;
class window_surface;
}

class elem_window {
public:
    virtual void set_layout(const char *fmt) = 0;
    virtual int attach(graphics::plot* p, const char* slot_str) = 0;
    virtual void slot_refresh(unsigned index) = 0;
    virtual void start(unsigned width, unsigned height, unsigned flags) = 0;
    virtual void wait() = 0;
    virtual void close() = 0;
    virtual graphics::window_surface *get_window_surface() = 0;
    virtual ~elem_window() { }
};
