#ifndef GRAPHICS_SG_ELEMENT_H
#define GRAPHICS_SG_ELEMENT_H

#include "agg_color_rgba.h"
#include "agg_array.h"
#include "agg_conv_transform.h"
#include "agg_conv_stroke.h"

#include "sg_object.h"
#include "colors.h"
#include "trans.h"

inline static bool color_is_defined(agg::rgba8 c) {
    return (c.a > 0);
}

namespace graphics { namespace property {

enum {
    fill        = 1,
    stroke      = 1 << 1,
    crisp       = 1 << 2,
    fill_stroke = fill | stroke,
};

}}

class sg_element {
public:
    sg_element(): m_obj(0) { }

    sg_element(sg_object* o, agg::rgba8 fc, agg::rgba8 sc, float sw, unsigned flags = graphics::property::fill_stroke):
    m_obj(o), m_stroke_color(sc), m_stroke_width(sw), m_fill_color(fc),
    m_flags(flags)
    { }

    sg_object* object() const { return m_obj; }

    template <class Canvas>
    void draw(Canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb = 0)
    {
        m_obj->apply_transform(m, 1.0);
        const bool has_fill = (m_flags & graphics::property::fill);
        const bool has_stroke = (m_stroke_width > 0.0 && (m_flags & graphics::property::stroke));
        const bool crisp = (m_flags & graphics::property::crisp);
        if (has_fill) {
            canvas.draw(*m_obj, m_fill_color);
        }
        if (has_stroke) {
            if (crisp) {
                canvas.draw_outline_noaa(*m_obj, m_stroke_color);
            } else {
                graphics::transform::stroke_a stobj(m_obj);
                stobj.self().width(double(m_stroke_width));
                stobj.self().line_cap(agg::round_cap);
                canvas.draw(stobj, m_stroke_color);
            }
        }
        if (bb) {
            if (has_fill || has_stroke) {
                agg::bounding_rect_single(*m_obj, 0, &bb->x1, &bb->y1, &bb->x2, &bb->y2);
            } else {
                bb->x1 =  0.0;
                bb->x2 = -1.0;
            }
        }
    }
private:
    sg_object* m_obj;
    agg::rgba8 m_stroke_color;
    float m_stroke_width;
    agg::rgba8 m_fill_color;
    unsigned m_flags;
};

class sg_composite {
public:
    sg_composite() {}

    ~sg_composite() {
        unsigned n = m_elements.size();
        for (unsigned i = 0; i < n; i++) {
            delete m_elements[i].object();
        }
    }

    void add(const sg_element& e) {
        m_elements.add(e);
    }

    void add(sg_object* obj, agg::rgba8 fill_color, agg::rgba8 stroke_color, float stroke_width, unsigned flags = 0) {
        const sg_element e(obj, fill_color, stroke_color, stroke_width, graphics::property::stroke|graphics::property::fill|flags);
        m_elements.add(e);
    }

    void add_fill(sg_object* obj, agg::rgba8 fill_color, unsigned flags = 0) {
        const sg_element e(obj, fill_color, colors::zero(), 0.0, graphics::property::fill|flags);
        m_elements.add(e);
    }

    void add_stroke(sg_object* obj, agg::rgba8 color, float line_width, unsigned flags = 0) {
        const sg_element e(obj, colors::zero(), color, line_width, graphics::property::stroke|flags);
        m_elements.add(e);
    }

    template <class Canvas>
    void draw(Canvas& canvas, const agg::trans_affine& m)
    {
        unsigned n = m_elements.size();
        for (unsigned i = 0; i < n; i++) {
            sg_element& e = m_elements[i];
            e.draw(canvas, m);
        }
    }
private:
    agg::pod_bvector<sg_element> m_elements;
};

#endif
