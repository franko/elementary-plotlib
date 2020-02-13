#pragma once

#include "image_buf.h"
#include "render_config.h"
#include "update_region_notify.h"

namespace graphics {

typedef image_gen<pixel_size, flip_y> image;

struct display_window {
    virtual bool update_region_request(int index) = 0;

    // should be protected so that it can be called only from the
    // window's thread.
    virtual void update_region(const image& img, const agg::rect_i& r) = 0;

    virtual ~display_window() { };
};

}
