#pragma once

#include <initializer_list>
#include <utility>

#include "agg_trans_affine.h"
#include "agg_path_storage.h"
#include "agg_conv_transform.h"

#include "sg_object.h"
#include "my_conv_simple_marker.h"

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


class markers : public sg_object {
public:
    markers(double size, sg_object* sym):
        m_path(), m_scaling_matrix(), m_scaling_trans(m_path, m_scaling_matrix),
        m_marker_conv(m_scaling_trans, *sym),
        m_size(size), m_scale(m_size), m_symbol(sym) {
        // we need to apply the scale transform here to ensure that
        // any call to bounding_box have the correct informations about
        // the symbol size, even if it is called before apply_transform
        m_symbol->apply_transform(m_scale, 1.0);
    }

    virtual ~markers() {
        delete m_symbol;
    }

    virtual void apply_transform(const agg::trans_affine& m, double as)
    {
        m_symbol->apply_transform(m_scale, as);
        m_scaling_matrix = m;
    }

    virtual void rewind(unsigned path_id) {
        m_marker_conv.rewind(path_id);
    }

    virtual unsigned vertex(double* x, double* y) {
        return m_marker_conv.vertex(x, y);
    }

    virtual void bounding_box(double *x1, double *y1, double *x2, double *y2) {
        agg::bounding_rect_single(m_scaling_trans, 0, x1, y1, x2, y2);
    }

    virtual sg_object *copy() const {
        sg_object *new_symbol = m_symbol->copy();
        markers *new_object = new markers(m_size, new_symbol);
        vertex_source_copy(new_object->m_path, m_path);
        return new_object;
    }

    virtual str write_svg(int id, agg::rgba8 c, double h) {
        str marker_id;
        str marker_def = gen_svg_marker_def(id, c, marker_id);

        str path;
        svg_coords_from_vs(&m_scaling_trans, path, h);

        str marker_url = gen_marker_url(marker_id);
        const char* murl = marker_url.cstr();
        svg_property_item item1(marker_start, murl);
        svg_property_item item2(marker_mid, murl);
        svg_property_item item3(marker_end, murl);
        auto ls = new svg_property_list(item1, nullptr);
        ls = new svg_property_list(item2, ls);
        ls = new svg_property_list(item3, ls);

        str svg = svg_marker_path(path, m_size, id, ls);
        svg_property_list::free(ls);

        return str::print("%s\n   %s", marker_def.cstr(), svg.cstr());
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

private:
    str gen_svg_marker_def(int id, agg::rgba8 c, str& marker_id) {
        const double pad = 2.0;
        double tx_save = m_scale.tx, ty_save = m_scale.ty;

        m_scale.tx = m_size / 2.0 + pad;
        m_scale.ty = m_size / 2.0 + pad;

        marker_id.printf("marker%i", id);

        const double S = m_size + 2*pad;
        const double wf = S / m_size;

        str marker_svg = m_symbol->write_svg(-1, c, S);

        str s = str::print("<defs><marker id=\"%s\" "
                           "refX=\"%g\" refY=\"%g\" "
                           "viewBox=\"0 0 %g %g\" orient=\"0\" "
                           "markerWidth=\"%g\" markerHeight=\"%g\">"
                           "%s"
                           "</marker></defs>",
                           marker_id.cstr(), S/2, S/2, S, S, wf, wf,
                           marker_svg.cstr());

        m_scale.tx = tx_save;
        m_scale.ty = ty_save;

        return s;
    }

    static str gen_marker_url(str& marker_id) {
        return str::print("url(#%s)", marker_id.cstr());
    }


private:
    agg::path_storage m_path;
    agg::trans_affine m_scaling_matrix;
    agg::conv_transform<agg::path_storage> m_scaling_trans;
    my::conv_simple_marker<agg::conv_transform<agg::path_storage>, sg_object> m_marker_conv;
    double m_size;
    agg::trans_affine_scaling m_scale;
    sg_object* m_symbol;
};

}
