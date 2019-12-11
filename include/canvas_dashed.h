#pragma once

#include "agg_array.h"
#include "agg_conv_transform.h"
#include "agg_conv_dash.h"

#include "canvas_path.h"

class elp_dashed : public elp_path {
public:
    elp_dashed() : elp_path(), m_path_dash(m_path_scaling), m_svg_dash_array(16) { }

    void rewind(unsigned path_id) override {
        m_path_dash.rewind(path_id);
    }

    unsigned vertex(double* x, double* y) override {
        return m_path_dash.vertex(x, y);
    }

    void add_dash(double a, double b) {
        m_path_dash.add_dash(a, b);
        m_dash_lengths.add(a);
        m_dash_lengths.add(b);
        m_svg_dash_array.append("", ',');
        m_svg_dash_array.printf_add("%g,%g", a, b);
    }

    svg_property_list* svg_path(str& s, double h) override {
        svg_coords_from_vs(&m_path_scaling, s, h);
        svg_property_item item(stroke_dasharray, m_svg_dash_array.cstr());
        return new svg_property_list(item, nullptr);
    }

    elp_object *copy() const override {
        elp_dashed *new_object = new elp_dashed{};
        vertex_source_copy(new_object->m_path, m_path);
        new_object->m_dash_lengths = m_dash_lengths;
        new_object->m_svg_dash_array = m_svg_dash_array;
        for (unsigned k = 0; k < m_dash_lengths.size(); k += 2) {
            float a = m_dash_lengths[k], b = m_dash_lengths[k + 1];
            new_object->m_path_dash.add_dash(a, b);
        }
        return new_object;
    }

protected:
    agg::conv_dash<agg::conv_transform<agg::path_storage>> m_path_dash;
    agg::pod_bvector<float> m_dash_lengths;
    str m_svg_dash_array;
};
