#pragma once

#include <initializer_list>
#include <utility>

#include "agg_trans_affine.h"
#include "agg_path_storage.h"
#include "agg_conv_transform.h"

#include "canvas_path.h"

class elem_curve : public elem_path_base {
public:
    elem_curve() : elem_path_base(), m_path_curve(m_path), m_scaling_matrix(), m_path_scaling(m_path_curve, m_scaling_matrix) { }

    virtual void apply_transform(const agg::trans_affine& m, double as) {
        m_scaling_matrix = m;
        m_path_curve.approximation_scale(as * m.scale());
    }

    virtual void rewind(unsigned path_id) {
        m_path_scaling.rewind(path_id);
    }

    virtual unsigned vertex(double* x, double* y) {
        return m_path_scaling.vertex(x, y);
    }

    virtual elem_object *copy() const {
        elem_curve *new_object = new elem_curve{};
        vertex_source_copy(new_object->m_path, m_path);
        return new_object;
    }

private:
    agg::conv_curve<agg::path_storage> m_path_curve;
    agg::trans_affine m_scaling_matrix;
    agg::conv_transform<agg::conv_curve<agg::path_storage>> m_path_scaling;
};
