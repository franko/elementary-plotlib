#pragma once

#include "agg_color_rgba.h"
#include "agg_array.h"
#include "agg_conv_transform.h"
#include "agg_conv_stroke.h"

#include "canvas_object.h"
#include "colors.h"
#include "trans.h"

inline static bool color_is_defined(agg::rgba8 c) {
    return (c.a > 0);
}

namespace graphics { namespace property {

enum {
    fill        = 1 << 0,
    stroke      = 1 << 1,
    outline     = 1 << 2,
    crisp       = 1 << 3,
};

}}

inline bool color_alpha_not_zero(const agg::rgba8& color) {
    return (color.a > 0);
}

struct sg_element {
    canvas_object* object;
    agg::rgba8 stroke_color;
    float      stroke_width;
    agg::rgba8 fill_color;
    unsigned   flags;

    void clone_object() {
        object = object->copy();
    }

    template <class Canvas>
    void draw(Canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb = 0)
    {
        object->apply_transform(m, 1.0);
        const bool has_fill = (flags & graphics::property::fill) && color_alpha_not_zero(fill_color);
        const bool has_stroke = stroke_width > 0.0 && (flags & graphics::property::stroke) && color_alpha_not_zero(stroke_color);
        const bool crisp = (flags & graphics::property::crisp);
        const bool outline = (flags & graphics::property::outline) && color_alpha_not_zero(stroke_color);
        if (has_fill) {
            if (crisp) {
                canvas.draw_noaa(*object, fill_color);
            } else {
                canvas.draw(*object, fill_color);
            }
        }
        if (outline) {
            canvas.draw_outline_noaa(*object, stroke_color);
        } else if (has_stroke) {
            graphics::transform::stroke_a stobj(object);
            stobj.width(double(stroke_width));
            stobj.self().line_cap(agg::round_cap);
            if (crisp) {
                canvas.draw_noaa(stobj, stroke_color);
            } else {
                canvas.draw(stobj, stroke_color);
            }
        }
        if (bb) {
            if (has_fill || has_stroke || outline) {
                agg::bounding_rect_single(*object, 0, &bb->x1, &bb->y1, &bb->x2, &bb->y2);
            } else {
                bb->x1 =  0.0;
                bb->x2 = -1.0;
            }
        }
    }
};

class sg_composite {
public:
    sg_composite() {}

    ~sg_composite() {
        unsigned n = m_elements.size();
        for (unsigned i = 0; i < n; i++) {
            delete m_elements[i].object;
        }
    }

    void add(const sg_element& e) {
        m_elements.add(e);
    }

    void add(canvas_object* obj, agg::rgba8 fill_color, agg::rgba8 stroke_color, float stroke_width, unsigned flags = 0) {
        const sg_element e{obj, stroke_color, stroke_width, fill_color, graphics::property::stroke|graphics::property::fill|flags};
        m_elements.add(e);
    }

    void add_fill(canvas_object* obj, agg::rgba8 fill_color, unsigned flags = 0) {
        const sg_element e{obj, colors::zero(), 0.0, fill_color, graphics::property::fill|flags};
        m_elements.add(e);
    }

    void add_stroke(canvas_object* obj, agg::rgba8 color, float line_width, unsigned flags = 0) {
        const sg_element e{obj, color, line_width, colors::zero(), graphics::property::stroke|flags};
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
