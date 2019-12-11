#include "canvas_path.h"
#include "my_conv_simple_marker.h"

class elp_markers : public elp_path {
public:
    elp_markers(double size, elp_object* sym) : elp_path(),
        m_marker_conv(m_path_scaling, *sym), m_size(size), m_scale(m_size), m_symbol(sym) {
        // we need to apply the scale transform here to ensure that
        // any call to bounding_box have the correct informations about
        // the symbol size, even if it is called before apply_transform
        m_symbol->apply_transform(m_scale, 1.0);
    }

    ~elp_markers() override {
        delete m_symbol;
    }

    void apply_transform(const agg::trans_affine& m, double as) override {
        path_base_scaling::apply_transform(m, as);
        m_symbol->apply_transform(m_scale, as);
    }

    void rewind(unsigned path_id) override {
        m_marker_conv.rewind(path_id);
    }

    unsigned vertex(double* x, double* y) override {
        return m_marker_conv.vertex(x, y);
    }

    elp_object *copy() const override {
        elp_object *new_symbol = m_symbol->copy();
        elp_markers *new_object = new elp_markers(m_size, new_symbol);
        vertex_source_copy(new_object->m_path, m_path);
        return new_object;
    }

    str write_svg(int id, agg::rgba8 c, double h) override {
        str marker_id;
        str marker_def = gen_svg_marker_def(id, c, marker_id);

        str path;
        svg_coords_from_vs(&m_path_scaling, path, h);

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
    my::conv_simple_marker<agg::conv_transform<agg::path_storage>, elp_object> m_marker_conv;
    double m_size;
    agg::trans_affine_scaling m_scale;
    elp_object* m_symbol;
};
