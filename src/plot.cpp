#include "plot.h"
#include "colors.h"
#include "sg_path.h"
#include "plot_style.h"

namespace graphics {

static double compute_scale(const agg::trans_affine& m)
{
    return m.scale() / 480.0;
}

void plot::commit_pending_draw()
{
    push_drawing_queue();
    m_need_redraw = false;
    m_changes_pending.clear();
}

void plot::add(sg_element elem)
{
    if (m_auto_limits && !fit_inside(elem)) {
        m_bbox_updated = false;
        m_need_redraw = true;
        m_enlarged_layer = true;
    }

    auto new_node = new list<sg_element>(elem);
    m_drawing_queue = list<sg_element>::push_back(m_drawing_queue, new_node);
}

void plot::push_drawing_queue()
{
    auto layer = current_layer();
    for (auto c = m_drawing_queue; c != 0; c = c->next())
    {
        layer->add(c->content());
    }

    while (m_drawing_queue) {
        m_drawing_queue = list<sg_element>::pop(m_drawing_queue);
    }
}

void plot::clear_drawing_queue()
{
    while (m_drawing_queue) {
        sg_element& d = m_drawing_queue->content();
        delete d.object;
        m_drawing_queue = list<sg_element>::pop(m_drawing_queue);
    }
}

static bool area_is_valid(const agg::trans_affine& b)
{
    const double thresold = 40.0;
    return (b.sx > thresold && b.sy > thresold);
}

void plot::draw_virtual_canvas(canvas_type& canvas, plot_layout& layout, const agg::rect_i* clip)
{
    before_draw();
    draw_legends(canvas, layout);

    if (area_is_valid(layout.plot_area))
    {
        draw_axis(canvas, layout, clip);
        draw_elements(canvas, layout);
    }
};

void plot::draw_simple(canvas_type& canvas, plot_layout& layout, const agg::rect_i* clip)
{
    before_draw();
    draw_axis(canvas, layout, clip);
    draw_elements(canvas, layout);
};

agg::trans_affine plot::get_model_matrix(const plot_layout& layout)
{
    agg::trans_affine m = m_trans;
    trans_affine_compose (m, layout.plot_active_area);
    return m;
}

void plot::clip_plot_area(canvas_type& canvas, const agg::trans_affine& area_mtx)
{
    if (this->clip_is_active())
    {
        agg::rect_base<int> clip = rect_of_slot_matrix<int>(area_mtx);
        canvas.clip_box(clip);
    }
}

void plot::draw_elements(canvas_type& canvas, const plot_layout& layout)
{
    const agg::trans_affine m = get_model_matrix(layout);

    clip_plot_area(canvas, layout.plot_active_area);

    for (unsigned k = 0; k < m_layers.size(); k++)
    {
        item_list& layer = *(m_layers[k]);
        for (unsigned j = 0; j < layer.size(); j++) {
            draw_element(layer[j], canvas, m);
        }
    }

    canvas.reset_clipping();
}

void plot::compute_user_trans()
{
    agg::rect_d rin = m_rect.is_defined() ? m_rect.rect() : agg::rect_d(0, 0, 1, 1);
    agg::rect_d r;
    m_x_axis.get_limits(rin.x1, rin.x2, r.x1, r.x2);
    m_y_axis.get_limits(rin.y1, rin.y2, r.y1, r.y2);
    const double dx = r.x2 - r.x1, dy = r.y2 - r.y1;
    const double fx = (dx == 0 ? 1.0 : 1/dx), fy = (dy == 0 ? 1.0 : 1/dy);
    this->m_trans = agg::trans_affine(fx, 0.0, 0.0, fy, -r.x1 * fx, -r.y1 * fy);
}

void plot::draw_grid(const axis_e dir, const units& u,
                         const agg::trans_affine& user_mtx,
                         agg::path_storage& ln)
{
    const double eps = 1.0e-3;
    const bool isx = (dir == x_axis);

    int jinf = u.begin(), jsup = u.end();
    for (int j = jinf+1; j < jsup; j++)
    {
        double uq = u.mark_value(j);
        double x = (isx ? uq : 0), y = (isx ? 0.0 : uq);
        user_mtx.transform(&x, &y);
        const double q = (isx ? x : y);

        if (q >= -eps && q <= 1.0 + eps)
        {
            ln.move_to(isx ? q : 0.0, isx ? 0.0 : q);
            ln.line_to(isx ? q : 1.0, isx ? 1.0 : q);
        }
    }
}

static inline double approx_text_height(double text_size)
{
    return text_size * 1.5;
}

plot_layout plot::compute_plot_layout(const agg::trans_affine& canvas_mtx, bool do_legends)
{
    plot_layout layout;

    const double sx = canvas_mtx.sx, sy = canvas_mtx.sy;
    const double ppad = double(canvas_margin_prop_space) / 1000.0;
    const double fpad = double(canvas_margin_fixed_space);
    // const double size_frac_x = 0.125, size_frac_y = 0.05;

    double dxl, dxr, dyb, dyt;

    dxl = dxr = fpad + ppad * sx;
    dyb = dyt = fpad + ppad * sy;

    if (!str_is_null(&m_title))
    {
        const double scale = compute_scale(canvas_mtx);
        const double ptpad = double(axis_title_prop_space) / 1000.0;
        const double title_text_size = get_default_font_size(text_plot_title, scale);
        const double th = approx_text_height(title_text_size);

        double x = 0.5, y = 1.0;
        canvas_mtx.transform(&x, &y);
        y -= ptpad + dyt + title_text_size;

        layout.title_pos = plot_layout::point(x, y);
        layout.title_font_size = title_text_size;

        dyt += 2 * ptpad + th;
    }

#if 0
    for (int k = 0; k < 4 && do_legends; k++)
    {
        plot* mp = m_legend[k];

        if (mp)
        {
            agg::rect_base<double> bb;
            mp->get_bounding_rect(bb);

            double bb_dx = bb.x2 - bb.x1, bb_dy = bb.y2 - bb.y1;
            double dx, dy;
            double px, py;
            switch (k)
            {
            case right:
                dx = max(sx * size_frac_x, bb_dx);
                dy = dx * bb_dy / bb_dx;
                px = sx - dx - ppad * sx - dxr;
                py = (sy - dy) / 2;
                dxr += dx + 2 * ppad * sx;
                break;
            case left:
                dx = max(sx * size_frac_x, bb_dx);
                dy = dx * bb_dy / bb_dx;
                px = ppad * sx + dxr;
                py = (sy - dy) / 2;
                dxl += dx + 2 * ppad * sx;
                break;
            case bottom:
                dy = sy * size_frac_y;
                dx = dy * bb_dx / bb_dy;
                py = ppad * sy + dyb;
                px = (sx - dx) / 2;
                dyb += dy + 2 * ppad * sy;
                break;
            case top:
            default:
                dy = sy * size_frac_y;
                dx = dy * bb_dx / bb_dy;
                py = sy - dy - ppad * sy - dyt;
                px = (sx - dx) / 2;
                dyt += dy + 2 * ppad * sy;
            }

            if (px >= 0 && py >= 0 && px + dx < sx && py + dy < sy)
            {
                const double x0 = canvas_mtx.tx + px, y0 = canvas_mtx.ty + py;
                layout.legend_area[k] = agg::trans_affine(dx, 0.0, 0.0, dy, x0, y0);
            }
            else
            {
                plot_layout::set_area_undefined(layout.legend_area[k]);
            }
        }
    }
#endif

    double x0 = canvas_mtx.tx + dxl, y0 = canvas_mtx.ty + dyb;
    double ssx = sx - (dxl + dxr), ssy = sy - (dyb + dyt);
    layout.plot_area = agg::trans_affine(ssx, 0.0, 0.0, ssy, x0, y0);

    return layout;
}

void plot::draw_legends(canvas_type& canvas, const plot_layout& layout)
{
    if (!str_is_null(&m_title))
    {
        const plot_layout::point& pos = layout.title_pos;
        graphics::text title(m_title.cstr(), layout.title_font_size, 0.5, 0.0);
        title.set_point(pos.x, pos.y);
        title.apply_transform(identity_matrix, 1.0);
        canvas.draw(title, colors::black());
    }

#if 0
    for (int k = 0; k < 4; k++)
    {
        plot* mp = m_legend[k];
        const agg::trans_affine& mtx = layout.legend_area[k];

        if (mp && plot_layout::is_area_defined(mtx))
        {
            agg::rect_i clip = rect_of_slot_matrix<int>(mtx);
            plot_layout mp_layout = mp->compute_plot_layout(mtx, false);
            mp->draw_simple(canvas, mp_layout, &clip);
        }
    }
#endif
}

// Draw the axis elements and labels and set layout.plot_active_area
// to the actual plotting are matrix.
void plot::draw_axis(canvas_type& canvas, plot_layout& layout, const agg::rect_i* clip)
{
    const double scale = compute_scale(layout.plot_area);

    if (clip)
        canvas.clip_box(*clip);

    const agg::trans_affine& m = layout.plot_active_area;

    const double label_text_size = get_default_font_size(text_axis_title, scale);
    const double plpad = double(axis_label_prop_space) / 1000.0;
    const double ptpad = double(axis_title_prop_space) / 1000.0;

    double dx_label, dy_label;
    // The calls below will set dy_label to the screen size needed for the axis
    // and the labels below.
    sg_composite x_axis_comp = m_x_axis.draw(m_trans, dy_label, scale);
    sg_composite y_axis_comp = m_y_axis.draw(m_trans, dx_label, scale);

    double ppad_left = plpad, ppad_right = plpad;
    double ppad_bottom = plpad, ppad_top = plpad;
    double dx_left = dx_label, dx_right = 0.0;
    double dy_bottom = dy_label, dy_top = 0.0;

    if (!str_is_null(&m_y_axis.title))
    {
        dx_left += approx_text_height(label_text_size);
        ppad_left += ptpad;
    }

    if (!str_is_null(&m_x_axis.title))
    {
        dy_bottom += approx_text_height(label_text_size);
        ppad_bottom += ptpad;
    }

    const double sx = layout.plot_area.sx, sy = layout.plot_area.sy;
    const double x0 = layout.plot_area.tx, y0 = layout.plot_area.ty;

    const double xppad = (ppad_left + ppad_right);
    const double lsx = (dx_left + dx_right + xppad * sx) / (1 + xppad);

    const double yppad = (ppad_bottom + ppad_top);
    const double lsy = (dy_bottom + dy_top + yppad * sy) / (1 + yppad);

    const double sxr = sx - lsx;
    const double syr = sy - lsy;

    const double aax = x0 + dx_left + ppad_left * sxr;
    const double aay = y0 + dy_bottom + ppad_bottom * syr;
    layout.set_plot_active_area(sxr, syr, aax, aay);

    const agg::trans_affine x_axis_mat(m.sx, 0.0, 0.0, 1.0,  m.tx, m.ty);
    const agg::trans_affine y_axis_mat(1.0,  0.0, 0.0, m.sy, m.tx, m.ty);

    x_axis_comp.draw(canvas, x_axis_mat);
    y_axis_comp.draw(canvas, y_axis_mat);

    if (!str_is_null(&m_x_axis.title))
    {
        double labx = m.sx * 0.5 + m.tx;
        double laby = y0;

        const char* text = m_x_axis.title.cstr();
        graphics::text xlabel(text, label_text_size, 0.5, 0.0);
        xlabel.set_point(labx, laby);
        xlabel.apply_transform(identity_matrix, 1.0);

        canvas.draw(xlabel, colors::black());
    }

    if (!str_is_null(&m_y_axis.title))
    {
        double labx = x0;
        double laby = m.sy * 0.5 + m.ty;

        const char* text = m_y_axis.title.cstr();
        graphics::text ylabel(text, label_text_size, 0.5, 1.0);
        ylabel.set_point(labx, laby);
        ylabel.angle(M_PI/2.0);
        ylabel.apply_transform(identity_matrix, 1.0);

        canvas.draw(ylabel, colors::black());
    }

    if (clip)
        canvas.reset_clipping();
}

void plot::set_axis_labels_angle(axis_e axis_dir, double angle)
{
    get_axis(axis_dir).set_labels_angle(angle);
    m_need_redraw = true;
    compute_user_trans();
}

void plot::set_units(bool _use_units)
{
    if (use_units() != _use_units)
    {
        m_x_axis.active = _use_units;
        m_y_axis.active = _use_units;
        m_need_redraw = true;
        compute_user_trans();
    }
}

void plot::update_units()
{
    units& ux = m_x_axis.u;
    units& uy = m_y_axis.u;
    if (m_rect.is_defined())
    {
        const rect_base<double>& r = m_rect.rect();
        ux.set(r.x1, r.x2);
        uy.set(r.y1, r.y2);
    }
    else
    {
        ux.clear();
        uy.clear();
    }

    compute_user_trans();
}

void plot::set_limits(const agg::rect_base<double>& r)
{
    m_rect.set(r);
    update_units();
    m_need_redraw = true;
}

void plot::unset_limits()
{
    m_rect.clear();
    update_units();
    m_need_redraw = true;
}

void plot::layer_dispose_elements(plot::item_list* layer)
{
    unsigned n = layer->size();
    for (unsigned k = 0; k < n; k++) {
        delete layer->at(k).object;
    }
}

bool plot::push_layer()
{
    if (m_layers.size() >= max_layers)
        return false;

    item_list *new_layer = new(std::nothrow) item_list();
    if (new_layer) {
        before_draw();
        push_drawing_queue();
        m_layers.add(new_layer);
    }

    if (m_auto_limits) {
        if (m_rect.is_defined()) {
            parent_layer()->set_bounding_box(m_rect.rect());
        }
        m_bbox_updated = true;
        m_enlarged_layer = false;
    }

    return (new_layer != nullptr);
}

bool plot::pop_layer()
{
    if (m_layers.size() <= 1)
        return false;

    unsigned n = m_layers.size();
    item_list* layer = m_layers[n-1];
    m_layers.inc_size(-1);
    layer_dispose_elements(layer);
    delete layer;

    clear_drawing_queue();
    m_need_redraw = true;

    if (m_auto_limits) {
        if (m_enlarged_layer)
            m_bbox_updated = false;
        m_enlarged_layer = true;
    }

    return true;
}

void plot::set_opt_limits(const opt_rect<double>& r) {
    if (r.is_defined()) {
        set_limits(r.rect());
    } else {
        unset_limits();
    }
}

void plot::clear_current_layer()
{
    item_list* current = current_layer();
    clear_drawing_queue();
    layer_dispose_elements(current);
    current->clear();
    m_changes_pending = m_changes_accu;
    m_changes_accu.clear();

    if (m_auto_limits) {
        if (m_enlarged_layer) {
            item_list* parent = parent_layer();
            if (parent) {
                set_opt_limits(parent->bounding_box());
            } else {
                unset_limits();
            }
        }
        m_bbox_updated = true;
        m_enlarged_layer = false;
    }
}

int plot::current_layer_index()
{
    return m_layers.size();
}

bool plot::fit_inside(const sg_element& elem) const
{
    if (!m_bbox_updated || !m_rect.is_defined()) {
        return false;
    }

    agg::rect_base<double> r;
    elem.object->bounding_box(&r.x1, &r.y1, &r.x2, &r.y2);
    const agg::rect_d& bb = m_rect.rect();
    return bb.hit_test(r.x1, r.y1) && bb.hit_test(r.x2, r.y2);
}


void plot::calc_layer_bounding_box(plot::item_list* layer, opt_rect<double>& rect)
{
    for (unsigned j = 0; j < layer->size(); j++) {
        sg_element& d = (*layer)[j];
        agg::rect_base<double> r;
        d.object->bounding_box(&r.x1, &r.y1, &r.x2, &r.y2);
        rect.add<rect_union>(r);
    }
}

void plot::calc_bounding_box()
{
    opt_rect<double> box;
    unsigned n = nb_layers();
    for (unsigned j = 0; j < n-1; j++) {
        box.add<rect_union>(get_layer(j)->bounding_box());
    }

    calc_layer_bounding_box(get_layer(n-1), box);
    for (auto t = m_drawing_queue; t; t = t->next())
    {
        const sg_element& d = t->content();
        agg::rect_d r;
        d.object->bounding_box(&r.x1, &r.y1, &r.x2, &r.y2);
        box.add<rect_union>(r);
    }

    m_rect = box;
}

void plot::check_bounding_box()
{
    calc_bounding_box();
    update_units();
    m_bbox_updated = true;
}

void plot::before_draw()
{
    if (m_auto_limits && !m_bbox_updated) {
        check_bounding_box();
    }
}

} /* namespace graphics */
