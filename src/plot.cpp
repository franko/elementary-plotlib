#include "plot.h"
#include "colors.h"
#include "sg_path.h"
#include "plot_style.h"

#include <algorithm>
#include <cmath>

namespace graphics {

static double compute_scale(const agg::trans_affine& m)
{
    return m.scale() / 480.0;
}

namespace {

constexpr double legend_padding = 8.0;

} // namespace

void plot::commit_pending_draw()
{
    push_drawing_queue();
    m_need_redraw = m_layout_dirty;
    m_changes_pending.clear();
}

void plot::add(std::unique_ptr<sg_drawable> elem)
{
    if (!elem) {
        throw std::invalid_argument("drawable must not be null");
    }
    const bool enlarge = m_auto_limits && !fit_inside(*elem);
    m_drawing_queue.add(std::move(elem));
    if (enlarge) {
        m_bbox_updated = false;
        m_need_redraw = true;
        m_enlarged_layer = true;
    }
}

void plot::push_drawing_queue()
{
    current_layer()->append(std::move(m_drawing_queue));
}

void plot::clear_drawing_queue()
{
    m_drawing_queue.clear();
}

static bool area_is_valid(const agg::trans_affine& b)
{
    const double thresold = 40.0;
    return (b.sx > thresold && b.sy > thresold);
}

void plot::draw_virtual_canvas(canvas_type& canvas, plot_layout& layout, const agg::rect_i* clip)
{
    before_draw();
    draw_title(canvas, layout);
    draw_outside_legends(canvas, layout);

    if (area_is_valid(layout.plot_area))
    {
        draw_axis(canvas, layout, clip);
        draw_elements(canvas, layout);
    }
    draw_inside_legends(canvas, layout);
}

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
        canvas.clip_logical_area(clip);
    }
}

void plot::draw_elements(canvas_type& canvas, const plot_layout& layout)
{
    const agg::trans_affine m = get_model_matrix(layout);

    clip_plot_area(canvas, layout.plot_active_area);

    for (auto& layer : m_layers)
    {
        for (auto& element : layer) {
            element->draw(canvas, m);
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

    if (do_legends) {
        auto place_side_legend = [&](int location, bool left) {
            legend* value = m_legend[location].get();
            if (!value || value->empty()) {
                return;
            }

            const legend_size size = value->measure(legend_direction::vertical);
            const double width = size.width + 2.0 * legend_padding;
            const double height = size.height + 2.0 * legend_padding;
            double x;
            if (left) {
                dxl += width;
                x = canvas_mtx.tx + dxl - width;
            } else {
                dxr += width;
                x = canvas_mtx.tx + sx - dxr;
            }
            const double y = canvas_mtx.ty + (sy - height) / 2.0;
            layout.legend_area[location] = agg::trans_affine(width, 0.0, 0.0,
                                                              height, x, y);
        };

        place_side_legend(plot_legend_right, false);
        place_side_legend(plot_legend_left, true);

        const double max_row_width = std::max(
            0.0, sx - dxl - dxr - 2.0 * legend_padding);
        auto place_row_legend = [&](int location, bool top) {
            legend* value = m_legend[location].get();
            if (!value || value->empty()) {
                return;
            }

            const legend_size size = value->measure(legend_direction::horizontal,
                                                     max_row_width);
            const double width = size.width + 2.0 * legend_padding;
            const double height = size.height + 2.0 * legend_padding;
            const double available_width = sx - dxl - dxr;
            const double x = canvas_mtx.tx + dxl +
                             (available_width - width) / 2.0;
            double y;
            if (top) {
                dyt += height;
                y = canvas_mtx.ty + sy - dyt;
            } else {
                y = canvas_mtx.ty + dyb;
                dyb += height;
            }
            layout.legend_area[location] = agg::trans_affine(width, 0.0, 0.0,
                                                              height, x, y);
        };

        place_row_legend(plot_legend_bottom, false);
        place_row_legend(plot_legend_top, true);
    }

    double x0 = canvas_mtx.tx + dxl, y0 = canvas_mtx.ty + dyb;
    double ssx = sx - (dxl + dxr), ssy = sy - (dyb + dyt);
    layout.plot_area = agg::trans_affine(ssx, 0.0, 0.0, ssy, x0, y0);

    return layout;
}

void plot::draw_title(canvas_type& canvas, const plot_layout& layout)
{
    if (!str_is_null(&m_title))
    {
        const plot_layout::point& pos = layout.title_pos;
        graphics::text title(m_title.cstr(), layout.title_font_size, 0.5, 0.0);
        title.set_point(pos.x, pos.y);
        title.apply_transform(identity_matrix, 1.0);
        canvas.draw(title, colors::black());
    }
}

void plot::draw_outside_legends(canvas_type& canvas, const plot_layout& layout)
{
    for (int k = plot_legend_right; k <= plot_legend_top; ++k) {
        legend* value = m_legend[k].get();
        const agg::trans_affine& area = layout.legend_area[k];
        if (!value || !plot_layout::is_area_defined(area)) {
            continue;
        }

        const agg::rect_i clip = rect_of_slot_matrix<int>(area);
        canvas.clip_logical_area(clip);
        const legend_direction direction =
            (k == plot_legend_bottom || k == plot_legend_top)
                ? legend_direction::horizontal
                : legend_direction::vertical;
        value->draw_outside(canvas, area, direction);
        canvas.reset_clipping();
    }
}

void plot::draw_inside_legends(canvas_type& canvas, const plot_layout& layout,
                               opt_rect<double>* painted_bounds)
{
    if (!plot_layout::is_area_defined(layout.plot_active_area)) {
        return;
    }

    if (clip_is_active()) {
        clip_plot_area(canvas, layout.plot_active_area);
    }

    for (int k = plot_legend_inside_top_left;
         k <= plot_legend_inside_right; ++k) {
        legend* value = m_legend[k].get();
        if (!value) {
            continue;
        }

        switch (k) {
        case plot_legend_inside_top_left:
            value->draw_inside(canvas, layout.plot_active_area,
                               legend_corner::top_left, painted_bounds);
            break;
        case plot_legend_inside_top_right:
            value->draw_inside(canvas, layout.plot_active_area,
                               legend_corner::top_right, painted_bounds);
            break;
        case plot_legend_inside_bottom_left:
            value->draw_inside(canvas, layout.plot_active_area,
                               legend_corner::bottom_left, painted_bounds);
            break;
        case plot_legend_inside_bottom_right:
            value->draw_inside(canvas, layout.plot_active_area,
                               legend_corner::bottom_right, painted_bounds);
            break;
        case plot_legend_inside_top:
            value->draw_inside_edge(canvas, layout.plot_active_area,
                                    legend_edge::top, painted_bounds);
            break;
        case plot_legend_inside_bottom:
            value->draw_inside_edge(canvas, layout.plot_active_area,
                                    legend_edge::bottom, painted_bounds);
            break;
        case plot_legend_inside_left:
            value->draw_inside_edge(canvas, layout.plot_active_area,
                                    legend_edge::left, painted_bounds);
            break;
        case plot_legend_inside_right:
            value->draw_inside_edge(canvas, layout.plot_active_area,
                                    legend_edge::right, painted_bounds);
            break;
        }
    }

    if (clip_is_active()) {
        canvas.reset_clipping();
    }
}

// Draw the axis elements and labels and set layout.plot_active_area
// to the actual plotting are matrix.
void plot::draw_axis(canvas_type& canvas, plot_layout& layout, const agg::rect_i* clip)
{
    const double scale = compute_scale(layout.plot_area);

    if (clip)
        canvas.clip_logical_area(*clip);

    const agg::trans_affine& m = layout.plot_active_area;

    const double label_text_size = get_default_font_size(text_axis_title, scale);
    const double plpad = double(axis_label_prop_space) / 1000.0;
    const double ptpad = double(axis_title_prop_space) / 1000.0;

    double dx_label, dy_label;
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

bool plot::push_layer()
{
    if (m_layers.size() >= max_layers)
        return false;

    try {
        m_layers.reserve(m_layers.size() + 1);
        before_draw();
        push_drawing_queue();
        m_layers.emplace_back();
    } catch (const std::bad_alloc&) {
        return false;
    }

    if (m_auto_limits) {
        if (m_rect.is_defined()) {
            parent_layer()->set_bounding_box(m_rect.rect());
        }
        m_bbox_updated = true;
        m_enlarged_layer = false;
    }

    return true;
}

bool plot::pop_layer()
{
    if (m_layers.size() <= 1)
        return false;

    m_layers.pop_back();

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

bool plot::fit_inside(const sg_drawable& elem) const
{
    const opt_rect<double> bounds = elem.data_bounds();
    if (!bounds.is_defined()) {
        return true;
    }
    if (!m_bbox_updated || !m_rect.is_defined()) {
        return false;
    }

    const agg::rect_d& r = bounds.rect();
    const agg::rect_d& bb = m_rect.rect();
    return bb.hit_test(r.x1, r.y1) && bb.hit_test(r.x2, r.y2);
}


void plot::calc_layer_bounding_box(plot::item_list* layer, opt_rect<double>& rect)
{
    for (const auto& d : *layer) {
        rect.add<rect_union>(d->data_bounds());
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
    for (const auto& d : m_drawing_queue)
    {
        box.add<rect_union>(d->data_bounds());
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
