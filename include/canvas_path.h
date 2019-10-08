#pragma once

#include <initializer_list>
#include <utility>

#include "agg_trans_affine.h"
#include "agg_path_storage.h"
#include "agg_conv_transform.h"

#include "canvas_object.h"

/* This class serve as a base class for derived classes that have a
   path and should implement the canvas_object interface.
   No virtual methods for canvas_object is implemented except, for
   convenience bounding_box. */
class canvas_path_base : public canvas_object {
public:
    canvas_path_base() : m_path() { }

    void line_to(double x, double y) {
        m_path.line_to(x, y);
    }

    void move_to(double x, double y) {
        m_path.move_to(x, y);
    }

    void curve3(double x_ctrl, double y_ctrl, double x_to, double y_to) {
        m_path.curve3(x_ctrl, y_ctrl, x_to, y_to);
    }

    void curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to) {
        m_path.curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to);
    }

    void arc_to(double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y) {
        m_path.arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
    }

    void close_polygon() {
        m_path.close_polygon();
    }

    void bounding_box(double *x1, double *y1, double *x2, double *y2) override {
        agg::bounding_rect_single(m_path, 0, x1, y1, x2, y2);
    }

    void path_from_initializer_list(std::initializer_list<std::pair<double, double>> lst) {
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

protected:
    agg::path_storage m_path;
};

/* The same of the canvas_path_base class but provide scaling and implement
   the related apply_transform method from canvas_object interface. */
class path_base_scaling : public canvas_path_base {
public:
    path_base_scaling() : canvas_path_base(), m_scaling_matrix(), m_path_scaling(m_path, m_scaling_matrix) { }

    void apply_transform(const agg::trans_affine& m, double as) override {
        m_scaling_matrix = m;
    }

protected:
    agg::trans_affine m_scaling_matrix;
    agg::conv_transform<agg::path_storage> m_path_scaling;
};

class canvas_path : public path_base_scaling {
public:
    canvas_path() : path_base_scaling() { }

    canvas_path(std::initializer_list<std::pair<double, double>> lst) : path_base_scaling() {
        path_from_initializer_list(lst);
    }

    void rewind(unsigned path_id) override {
        m_path_scaling.rewind(path_id);
    }

    unsigned vertex(double* x, double* y) override {
        return m_path_scaling.vertex(x, y);
    }

    canvas_object *copy() const override {
        canvas_path *new_object = new canvas_path();
        vertex_source_copy(new_object->m_path, m_path);
        return new_object;
    }
};
