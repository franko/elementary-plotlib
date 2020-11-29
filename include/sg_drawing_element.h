#pragma once

#include "drawing_element.h"
#include "sg_element.h"

class sg_drawing_element : public drawing_element {
public:
    sg_drawing_element(sg_element element): m_content{element} { }
    ~sg_drawing_element();

    void draw(virtual_canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb) override;
    void bounding_box(double *x1, double *y1, double *x2, double *y2) override;
    drawing_element *copy() override;
private:
    sg_element m_content;
};

