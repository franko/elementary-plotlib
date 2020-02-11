#pragma once

#include "image_buf.h"
#include "render_config.h"
#include "update_region_notify.h"

namespace graphics {

typedef image_gen<pixel_size, flip_y> image;

struct display_window {
    virtual update_status update_region_request(image& img, const agg::rect_i& r) = 0;
    virtual ~display_window() { };
};

}
