#include <agg_array.h>
#include <agg_basics.h>

#include "drawing_element.h"

class triangles_drawing_element : public drawing_element {
public:
    using float_type = float;
    using point_type = agg::point_base<float_type>;
    using triangle_type = agg::pod_auto_array<int, 3>;

    triangles_drawing_element(agg::rgba8 color);

    template <typename PointsArray>
    void set_points(const PointsArray& points) {
        const unsigned n = points.size();
        m_points.resize(n);
        for (unsigned i = 0; i < n; i++) {
            m_points[i] = points[i];
        }
    }

    template <typename TrianglesArray>
    void set_triangles(const TrianglesArray& triangles) {
        const unsigned n = m_triangles.size();
        m_triangles.resize(n);
        for (unsigned i = 0; i < n; i++) {
            for (int k = 0; k < 3; k++) {
                m_triangles[i][k] = triangles[i][k];
            }
        }
    }

    // unsigned points_number() const { return m_points.size(); }
    // unsigned triangles_number() const { return m_triangles.size(); }

    void resize_points(int n) {
        if (n < 0) return;
        m_points.resize((unsigned) n);
    }

    void resize_triangles(int n) {
        if (n < 0) return;
        m_triangles.resize((unsigned) n);
    }

    const point_type& point(unsigned i) const { return m_points[i]; }
          point_type& point(unsigned i)       { return m_points[i]; }

    const triangle_type& triangle(unsigned i) const { return m_triangles[i]; }
          triangle_type& triangle(unsigned i)       { return m_triangles[i]; }


    void draw(virtual_canvas& canvas, const agg::trans_affine& m, agg::rect_d* bb) override;
    void bounding_box(double *x1, double *y1, double *x2, double *y2) override;
    drawing_element *clone() override;

private:
    agg::pod_array<point_type> m_points;
    agg::pod_array<triangle_type> m_triangles;
    agg::rgba8 m_color;
};

