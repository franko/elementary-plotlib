#ifndef GRAPHICS_SG_ELEMENT_H
#define GRAPHICS_SG_ELEMENT_H

#include "agg_color_rgba.h"
#include "agg_array.h"
#include "agg_conv_transform.h"
#include "agg_conv_stroke.h"

#include "sg_object.h"

enum sg_property_e {
    prop_stroke_color,
    prop_stroke_width,
    prop_fill_color,
};

enum sg_value_e {
    value_none,
    value_color,
    value_number,
    value_string,
};

struct sg_color {
    agg::int8u r, g, b, a;
};

struct sg_property {
    short name;
    short value_type;
    union {
        sg_color color;
        float number;
        str* text;
    } value;
};

static bool color_is_defined(agg::rgba8 c)
{
    return (c.a > 0);
}

struct sg_element {
    sg_element(): obj(0) { }

    sg_element(sg_object* o, agg::rgba8 fc, agg::rgba8 sc, float sw):
    obj(o), stroke_color(sc), stroke_width(sw), fill_color(fc)
    { }

    sg_object* obj;
    agg::rgba8 stroke_color;
    float stroke_width;
    agg::rgba8 fill_color;
    agg::pod_bvector<sg_property> property;

    template <class Canvas>
    void draw(Canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb = 0)
    {
        obj->apply_transform(m, 1.0);
        const bool has_fill = (color_is_defined(fill_color));
        const bool has_stroke = (stroke_width > 0.0 && color_is_defined(stroke_color));
        if (has_fill) {
            canvas.draw(*obj, fill_color);
            if (bb && !has_stroke) {
                agg::bounding_rect_single(*obj, 0, &bb->x1, &bb->y1, &bb->x2, &bb->y2);
            }
        }
        if (has_stroke) {
            trans::stroke_a stobj(obj);
            stobj.self().width(double(stroke_width));
            stobj.self().line_cap(agg::round_cap);
            canvas.draw(stobj, stroke_color);
            if (bb) {
                agg::bounding_rect_single(stobj, 0, &bb->x1, &bb->y1, &bb->x2, &bb->y2);
            }
        }
        if (bb && !has_fill && !has_stroke) {
            bb->x1 = 0.0;
            bb->x2 = -1.0;
        }
    }
};

#endif
