#include "sg_drawing_element.h"

sg_drawing_element::~sg_drawing_element() {
    delete m_content.object;
}

void sg_drawing_element::draw(virtual_canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb) {
    m_content.draw(canvas, m, bb);
}

void sg_drawing_element::bounding_box(double *x1, double *y1, double *x2, double *y2) {
    m_content.object->bounding_box(x1, y1, x2, y2);
};

drawing_element *sg_drawing_element::copy() {
    sg_drawing_element *new_element = new sg_drawing_element(m_content);
    new_element->m_content.clone_object();
    return new_element;
}

