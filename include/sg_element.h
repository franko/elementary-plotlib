#pragma once

#include "agg_color_rgba.h"
#include <cmath>
#include "agg_conv_transform.h"
#include "agg_conv_stroke.h"

#include "canvas_object.h"
#include "colors.h"
#include "trans.h"
#include "sg_drawable.h"

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

class sg_element : public sg_drawable {
public:
    sg_element(elem_object* object, agg::rgba8 stroke_color, float stroke_width,
               agg::rgba8 fill_color, unsigned flags,
               const agg::trans_affine& local_to_parent = agg::trans_affine()) :
        object(object), stroke_color(stroke_color), stroke_width(stroke_width),
        fill_color(fill_color), flags(flags), local_to_parent(local_to_parent) {
        if (!object) {
            throw std::invalid_argument("drawable geometry must not be null");
        }
    }

    sg_element(const sg_element& source) :
        sg_element(source.object->copy(), source.stroke_color, source.stroke_width,
                   source.fill_color, source.flags, source.local_to_parent) { }
    sg_element(sg_element&&) noexcept = default;
    sg_element& operator=(sg_element&&) noexcept = default;
    sg_element& operator=(const sg_element& source) {
        if (this != &source) {
            *this = sg_element(source);
        }
        return *this;
    }

    std::unique_ptr<sg_drawable> clone() const override {
        return std::make_unique<sg_element>(*this);
    }

    opt_rect<double> data_bounds() const override {
        agg::rect_d r;
        object->bounding_box(&r.x1, &r.y1, &r.x2, &r.y2);
        opt_rect<double> result;
        if (!r.is_valid()) {
            return result;
        }
        const double xs[] = {r.x1, r.x2, r.x2, r.x1};
        const double ys[] = {r.y1, r.y1, r.y2, r.y2};
        for (unsigned i = 0; i < 4; ++i) {
            double x = xs[i], y = ys[i];
            local_to_parent.transform(&x, &y);
            if (!std::isfinite(x) || !std::isfinite(y)) {
                return opt_rect<double>();
            }
            result.add<rect_union>(agg::rect_d(x, y, x, y));
        }
        return result;
    }

    void draw(virtual_canvas& canvas, const agg::trans_affine& parent_to_screen,
              opt_rect<double>* painted_bounds = nullptr) override
    {
        // AGG multiplication applies the left transform first.
        agg::trans_affine m = local_to_parent;
        m.multiply(parent_to_screen);
        object->apply_transform(m, 1.0);
        if (painted_bounds) {
            painted_bounds->clear();
        }
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
            include_painted_bounds(*object, painted_bounds);
        }
        if (outline) {
            canvas.draw_outline_noaa(*object, stroke_color);
            include_painted_bounds(*object, painted_bounds, 1.0);
        } else if (has_stroke) {
            graphics::transform::stroke_a stobj(object.get());
            stobj.width(double(stroke_width));
            stobj.self().line_cap(agg::round_cap);
            if (crisp) {
                canvas.draw_noaa(stobj, stroke_color);
            } else {
                canvas.draw(stobj, stroke_color);
            }
            include_painted_bounds(stobj, painted_bounds);
        }
    }

private:
    static void include_painted_bounds(elem_object& geometry, opt_rect<double>* bounds,
                                       double padding = 0.0) {
        if (!bounds) {
            return;
        }
        agg::rect_d r;
        if (agg::bounding_rect_single(geometry, 0, &r.x1, &r.y1, &r.x2, &r.y2)) {
            r.x1 -= padding;
            r.y1 -= padding;
            r.x2 += padding;
            r.y2 += padding;
            bounds->add<rect_union>(r);
        }
    }

    std::unique_ptr<elem_object> object;
    agg::rgba8 stroke_color;
    float stroke_width;
    agg::rgba8 fill_color;
    unsigned flags;
    agg::trans_affine local_to_parent;
};

class sg_composite {
public:
    void add(sg_element e) {
        m_elements.add(std::make_unique<sg_element>(std::move(e)));
    }

    void add(elem_object* obj, agg::rgba8 fill_color, agg::rgba8 stroke_color, float stroke_width, unsigned flags = 0) {
        add(sg_element{obj, stroke_color, stroke_width, fill_color, graphics::property::stroke|graphics::property::fill|flags});
    }

    void add_fill(elem_object* obj, agg::rgba8 fill_color, unsigned flags = 0) {
        add(sg_element{obj, colors::zero(), 0.0, fill_color, graphics::property::fill|flags});
    }

    void add_stroke(elem_object* obj, agg::rgba8 color, float line_width, unsigned flags = 0) {
        add(sg_element{obj, color, line_width, colors::zero(), graphics::property::stroke|flags});
    }

    void draw(virtual_canvas& canvas, const agg::trans_affine& m)
    {
        for (auto& e : m_elements) {
            e->draw(canvas, m);
        }
    }
private:
    sg_drawable_list m_elements;
};
