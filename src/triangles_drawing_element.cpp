#include <agg_trans_affine.h>
#include <agg_bounding_rect.h>
#include <agg_conv_transform.h>

#include "triangles_drawing_element.h"

template <typename PointsArray, typename TrianglesArray>
class triangles_storage {
public:
    triangles_storage(PointsArray& points, TrianglesArray& triangles):
        m_triangle_index(0),
        m_vertex_index(0),
        m_points(points),
        m_triangles(triangles)
    { }

    void rewind(unsigned path_id) {
        m_triangle_index = 0;
        m_vertex_index = 0;
    }

    unsigned vertex(double* x, double* y) {
        if (m_vertex_index > 3) {
            m_vertex_index = 0;
            m_triangle_index++;
        }
        if (m_triangle_index >= m_triangles.size()) {
            return agg::path_cmd_stop;
        }
        const auto& triangle = m_triangles[m_triangle_index];
        if (m_vertex_index < 3) {
            const auto& vertex = m_points[triangle[m_vertex_index]];
            *x = double(vertex.x);
            *y = double(vertex.y);
            return (m_vertex_index++ == 0 ? agg::path_cmd_move_to : agg::path_cmd_line_to);
        }
        m_vertex_index++;
        return agg::path_cmd_end_poly | agg::path_flags_close;
    }
private:
    unsigned m_triangle_index;
    unsigned m_vertex_index;
    PointsArray& m_points;
    TrianglesArray& m_triangles;
};

template <class VertexSource>
class elem_object_scaling_gen : public elem_object {
protected:
    VertexSource m_base;
    agg::trans_affine& m_mtx;
    agg::conv_transform<VertexSource> m_trans;

public:
    template <typename ...Args>
    elem_object_scaling_gen(agg::trans_affine& m, Args&&... args): m_base(std::forward<Args>(args)...), m_mtx(m), m_trans(m_base, m_mtx) { }

    void rewind(unsigned path_id) override {
        m_trans.rewind(path_id);
    }

    unsigned vertex(double* x, double* y) override {
        return m_trans.vertex(x, y);
    }

    void apply_transform(const agg::trans_affine& m, double as) override {
        m_mtx = m;
    }

    void bounding_box(double *x1, double *y1, double *x2, double *y2) override {
        agg::bounding_rect_single(m_base, 0, x1, y1, x2, y2);
    }

    elem_object *copy() const override {
        return nullptr;
    }
};

void triangles_drawing_element::draw(virtual_canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb) {
    agg::trans_affine mtx;
    elem_object_scaling_gen<triangles_storage<agg::pod_array<point_type>, agg::pod_array<triangle_type>>> triangles(mtx, m_points, m_triangles);
    triangles.apply_transform(m, 1.0);
    canvas.draw_noaa(triangles, m_color);
    if (bb) {
        triangles.bounding_box(&bb->x1, &bb->y1, &bb->x2, &bb->y2);
    }
}

void triangles_drawing_element::bounding_box(double *x1, double *y1, double *x2, double *y2) {
    triangles_storage<agg::pod_array<point_type>, agg::pod_array<triangle_type>> triangles(m_points, m_triangles);
    agg::bounding_rect_single(triangles, 0, x1, y1, x2, y2);
}

drawing_element *triangles_drawing_element::copy() {
    triangles_drawing_element *new_object = new triangles_drawing_element(m_color);
    new_object->set_points(m_points);
    new_object->set_triangles(m_triangles);
    return new_object;
}

