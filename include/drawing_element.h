#pragma once

#include <agg_basics.h>
#include <agg_trans_affine.h>

#include "canvas.h"

struct drawing_element {
    virtual void draw(virtual_canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb) = 0;
    virtual void bounding_box(double *x1, double *y1, double *x2, double *y2) = 0;
    virtual drawing_element *copy() = 0;
    virtual ~drawing_element() { }
};
