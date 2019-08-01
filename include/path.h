#pragma once

#include <initializer_list>
#include <utility>

#include "sg_path.h"
#include "sg_object.h"
#include "trans.h"

namespace graphics {

class path : public sg_object {
public:
    path() : m_path(), m_scaling_matrix(), m_scaling_trans(m_path, m_scaling_matrix) { }

    path(std::initializer_list<std::pair<double, double>> lst): path() {
        bool first = true;
        for (const auto& v : lst) {
            if (first) {
                m_path.move_to(v.first, v.second);
                first = false;
            } else {
                m_path.line_to(v.first, v.second);
            }
        }
    }

    virtual void rewind(unsigned path_id) {
        m_scaling_trans.rewind(path_id);
    }

    virtual unsigned vertex(double* x, double* y) {
        return m_scaling_trans.vertex(x, y);
    }

    virtual void apply_transform(const agg::trans_affine& m, double as) {
        m_scaling_matrix = m;
    }

    virtual void bounding_box(double *x1, double *y1, double *x2, double *y2) {
        agg::bounding_rect_single(m_path, 0, x1, y1, x2, y2);
    }

    void line_to(double x, double y) {
        m_path.line_to(x, y);
    }

    void move_to(double x, double y) {
        m_path.move_to(x, y);
    }

    void close_polygon() {
        m_path.close_polygon();
    }

    virtual sg_object *copy() const {
        path *new_object = new path();
        vertex_source_copy(new_object->m_path, m_path);
        return new_object;
    }

private:
    agg::path_storage m_path;
    agg::trans_affine m_scaling_matrix;
    agg::conv_transform<agg::path_storage> m_scaling_trans;
};

class polygon : public path {
public:
    polygon(): path() { }
    polygon(std::initializer_list<std::pair<double, double>> lst): path(lst) {
        close_polygon();
    }
};

class markers : public graphics::transform::marker_a {
public:
    markers(double size, sg_object* sym): graphics::transform::marker_a(&m_scaling_path, size, sym), m_scaling_path(&m_path) { }

    void line_to(double x, double y) {
        m_path.self().line_to(x, y);
    }

    void move_to(double x, double y) {
        m_path.self().move_to(x, y);
    }

    void close_polygon() {
        m_path.self().close_polygon();
    }

private:
    graphics::transform::scaling_a m_scaling_path;
    sg_path m_path;
};

}
