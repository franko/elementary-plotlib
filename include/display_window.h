#pragma once

#include "image_buf.h"
#include "render_config.h"
#include "update_region_notify.h"

namespace graphics {

typedef image_gen<pixel_size, flip_y> image;

enum class window_request {
    update,
    close,
};

struct display_window {
    virtual bool send_request(window_request request_type, int plot_index) = 0;

    // should be protected so that it can be called only from the
    // window's thread.
    virtual void update_region(const image& img, const agg::rect_i& r) = 0;

    virtual ~display_window() { };
};

}
