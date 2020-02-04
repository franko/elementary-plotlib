#pragma once

#include "image_buf.h"
#include "render_config.h"

namespace graphics {

typedef image_gen<pixel_size, flip_y> image;

class display_window {
public:
    virtual void update_region_request(image& img, const agg::rect_i& r) = 0;
    virtual ~display_window() { };
protected:
    virtual void update_region(const image& img, const agg::rect_i& r) = 0;
};

}
