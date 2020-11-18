#pragma once

#include <agg_trans_affine.h>

#include "canvas.h"

struct drawing_element {
    virtual void draw(virtual_canvas& canvas, const agg::trans_affine& m) = 0;
    virtual void bounding_box(double *x1, double *y1, double *x2, double *y2) = 0;
    virtual drawing_element *clone() = 0;
    virtual ~drawing_element() { }
};
