#pragma once

#include "image_buf.h"
#include "render_config.h"

namespace graphics {

typedef image_gen<pixel_size, flip_y> image;

struct display_window {
    virtual void update_region(image& img, const agg::rect_i& r) = 0;
    virtual void update_region_request(image& img, const agg::rect_i& r) = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual int status() = 0;
    virtual ~display_window() { };
};

}
