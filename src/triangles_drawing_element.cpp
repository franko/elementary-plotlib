#include <agg_basics.h>
#include <agg_bounding_rect.h>
#include <agg_conv_transform.h>
#include <agg_trans_affine.h>

#include "triangles_drawing_element.h"

template <typename PointsArray, typename TriangleType>
class triangle_vertex_source {
public:
    triangle_vertex_source(const PointsArray& points, const TriangleType& triangle):
        m_triangle(triangle),
        m_vertex_index(0),
        m_points(points)
    { }

    void rewind(unsigned path_id) {
        m_vertex_index = 0;
    }

    unsigned vertex(double* x, double* y) {
        if (m_vertex_index > 3) {
            return agg::path_cmd_stop;
        }
        if (m_vertex_index < 3) {
            const auto& vertex = m_points[m_triangle[m_vertex_index]];
            *x = double(vertex.x);
            *y = double(vertex.y);
            return (m_vertex_index++ == 0 ? agg::path_cmd_move_to : agg::path_cmd_line_to);
        }
        m_vertex_index++;
        return agg::path_cmd_end_poly | agg::path_flags_close;
    }
private:
    const TriangleType& m_triangle;
    unsigned m_vertex_index;
    const PointsArray& m_points;
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
    for (unsigned i = 0; i < m_triangles.size(); i++) {
        elem_object_scaling_gen<triangle_vertex_source<agg::pod_array<point_type>, triangle_type>> triangle(mtx, m_points, m_triangles[i]);
        triangle.apply_transform(m, 1.0);
        canvas.draw_noaa(triangle, m_colors[i]);
        if (bb) {
            // FIXME: it is probably more efficient to avoid using
            // the vertex_source instance and just loop over the points
            // corresponding to triangles' vertices.
            if (i == 0) {
                agg::bounding_rect_single(triangle, 0, &bb->x1, &bb->y1, &bb->x2, &bb->y2);
            } else {
                agg::rect_d tbb;
                agg::bounding_rect_single(triangle, 0, &tbb.x1, &tbb.y1, &tbb.x2, &tbb.y2);
                *bb = agg::unite_rectangles(*bb, tbb);
            }
        }
    }
}

void triangles_drawing_element::bounding_box(double *x1, double *y1, double *x2, double *y2) {
    agg::rect_d bb;
    for (unsigned i = 0; i < m_triangles.size(); i++) {
        triangle_vertex_source<agg::pod_array<point_type>, triangle_type> triangle(m_points, m_triangles[i]);
        // FIXME: it is probably more efficient to avoid using
        // the vertex_source instance and just loop over the points
        // corresponding to triangles' vertices.
        if (i == 0) {
            agg::bounding_rect_single(triangle, 0, &bb.x1, &bb.y1, &bb.x2, &bb.y2);
        } else {
            agg::rect_d tbb;
            agg::bounding_rect_single(triangle, 0, &tbb.x1, &tbb.y1, &tbb.x2, &tbb.y2);
            bb = agg::unite_rectangles(bb, tbb);
        }
    }
    *x1 = bb.x1;
    *y1 = bb.y1;
    *x2 = bb.x2;
    *y2 = bb.y2;
}

drawing_element *triangles_drawing_element::copy() {
    triangles_drawing_element *new_object = new triangles_drawing_element();
    new_object->set_points(m_points);
    new_object->set_triangles(m_triangles, m_colors);
    return new_object;
}

