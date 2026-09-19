
/* plot.h
 *
 * Copyright (C) 2009-2011 Francesco Abbate
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#include <new>
#include <mutex>

#include "utils.h"
#include "strpp.h"
#include "canvas.h"
#include "units.h"
#include "rect.h"
#include "sg_element.h"
#include "legend.h"
#include "text.h"
#include "plot_axis.h"

#include "agg_array.h"
#include "agg_bounding_rect.h"
#include "agg_color_rgba.h"
#include "agg_path_storage.h"

template <class Canvas>
class canvas_adapter : public virtual_canvas {
public:
    canvas_adapter(Canvas* c) : m_canvas(c) {}

    virtual void draw(elem_object& vs, agg::rgba8 c) {
        m_canvas->draw(vs, c);
    }

    virtual void draw_outline_noaa(elem_object& vs, agg::rgba8 c) {
        m_canvas->draw_outline_noaa(vs, c);
    }

    virtual void draw_noaa(elem_object& vs, agg::rgba8 c) {
        m_canvas->draw_noaa(vs, c);
    }

    virtual void clip_logical_area(const agg::rect_base<int>& clip) {
        m_canvas->clip_logical_area(clip);
    }

    virtual void reset_clipping() {
        m_canvas->reset_clipping();
    }

private:
    Canvas* m_canvas;
};

enum plot_legend_location_e {
    plot_legend_right = 0,
    plot_legend_left = 1,
    plot_legend_bottom = 2,
    plot_legend_top = 3,
    plot_legend_inside_top_left = 4,
    plot_legend_inside_top_right = 5,
    plot_legend_inside_bottom_left = 6,
    plot_legend_inside_bottom_right = 7,
    plot_legend_inside_top = 8,
    plot_legend_inside_bottom = 9,
    plot_legend_inside_left = 10,
    plot_legend_inside_right = 11,
    plot_legend_location_count = 12,
};

struct plot_layout {
    plot_layout() {
        for (int k = 0; k < plot_legend_location_count; ++k) {
            set_area_undefined(legend_area[k]);
        }
        set_area_undefined(plot_area);
        set_area_undefined(plot_active_area);
    }

    struct point {
        point(double _x, double _y): x(_x), y(_y) {}
        point() {}
        double x, y;
    };

    void set_plot_active_area(double sx, double sy, double tx, double ty)
    {
        plot_active_area.sx = sx;
        plot_active_area.sy = sy;
        plot_active_area.tx = tx;
        plot_active_area.ty = ty;
    }

    static void set_area_undefined(agg::trans_affine& m) {
        m.sx = -1.0;
    }

    static bool is_area_defined(const agg::trans_affine& m)
    {
        return (m.sx > 0.0);
    }

    point title_pos;
    double title_font_size;

    agg::trans_affine legend_area[plot_legend_location_count];
    agg::trans_affine plot_area;
    agg::trans_affine plot_active_area;
};

struct plot_render_info {
    agg::trans_affine active_area;
    unsigned layout_generation = 0;
};

namespace graphics {

class plot {

    static const unsigned max_layers = 8;

    enum {
        axis_label_prop_space = 20,
        axis_title_prop_space = 30,
        canvas_margin_prop_space = 15,
        canvas_margin_fixed_space = 4,
    };

protected:
    class item_list : public sg_drawable_list
    {
    public:
        item_list() = default;
        item_list(const item_list&) = default;
        item_list(item_list&& source) noexcept :
            sg_drawable_list(std::move(source)), m_bbox(source.m_bbox) { }

        const opt_rect<double>& bounding_box() const {
            return m_bbox;
        }

        void set_bounding_box(const agg::rect_base<double>& r) {
            m_bbox.set(r);
        }

        void clear_bounding_box() {
            m_bbox.clear();
        }

    private:
        opt_rect<double> m_bbox;
    };

public:
    typedef virtual_canvas canvas_type;

    enum { show_units = 1 << 0, auto_limits = 1 << 1, clip_region = 1 << 2 };

    class drawing_context {
    public:
        drawing_context(plot& p): m_plot(p) {
            m_plot.m_drawing_mutex.lock();
        }
        ~drawing_context() {
            m_plot.m_drawing_mutex.unlock();
        }
    private:
        plot& m_plot;
    };

    plot(unsigned flags) :
        m_clip_flag(flags & clip_region), m_need_redraw(true), m_layers(1),
        m_x_axis(x_axis, flags & show_units), m_y_axis(y_axis, flags & show_units),
        m_auto_limits(flags & auto_limits),
        m_bbox_updated(true), m_enlarged_layer(false), m_layout_dirty(false)
    {
        compute_user_trans();
    };


    plot(const plot& source) :
            m_trans(source.m_trans), m_drawing_queue(source.m_drawing_queue), m_clip_flag(source.m_clip_flag),
            m_need_redraw(source.m_need_redraw), m_rect(source.m_rect),
            m_changes_accu(source.m_changes_accu), m_changes_pending(source.m_changes_pending),
            m_layers(source.m_layers),
            m_title(source.m_title), m_x_axis(source.m_x_axis), m_y_axis(source.m_y_axis),
            m_auto_limits(source.m_auto_limits), m_bbox_updated(source.m_bbox_updated),
            m_enlarged_layer(source.m_enlarged_layer), m_layout_dirty(source.m_layout_dirty),
            m_layout_generation(source.m_layout_generation),
            m_drawing_mutex() {
        for (unsigned k = 0; k < plot_legend_location_count; k++) {
            const legend *source_legend = source.m_legend[k].get();
            if (source_legend) {
                m_legend[k] = std::make_unique<legend>(*source_legend);
            }
        }
    }

    plot(plot&& source) :
            m_trans(source.m_trans), m_clip_flag(source.m_clip_flag),
            m_need_redraw(source.m_need_redraw), m_rect(source.m_rect),
            m_changes_accu(source.m_changes_accu), m_changes_pending(source.m_changes_pending),
            m_layers(1),
            m_title(source.m_title), m_x_axis(source.m_x_axis), m_y_axis(source.m_y_axis),
            m_auto_limits(source.m_auto_limits), m_bbox_updated(source.m_bbox_updated),
            m_enlarged_layer(source.m_enlarged_layer), m_layout_dirty(source.m_layout_dirty),
            m_layout_generation(source.m_layout_generation),
            m_drawing_mutex() {
        m_drawing_queue = std::move(source.m_drawing_queue);
        m_layers.swap(source.m_layers);
        for (unsigned k = 0; k < plot_legend_location_count; k++) {
            m_legend[k] = std::move(source.m_legend[k]);
        }
    }

    ~plot() = default;

    str& title() {
        return m_title;
    }

    void set_title(const char *title) {
        set_string_property(m_title, title);
    }

    void set_x_axis_title(const char *title) {
        set_string_property(m_x_axis.title, title);
    }

    void set_y_axis_title(const char *title) {
        set_string_property(m_y_axis.title, title);
    }

    str& x_axis_title() {
        return m_x_axis.title;
    }
    str& y_axis_title() {
        return m_y_axis.title;
    }

    void set_legend(std::unique_ptr<legend> value, int where) {
        if (where < 0 || where >= plot_legend_location_count) {
            throw std::invalid_argument("invalid legend location");
        }
        m_legend[where] = std::move(value);
        m_layout_dirty = true;
        m_need_redraw = true;
        ++m_layout_generation;
    }

    void clear_legend(int where) {
        if (where < 0 || where >= plot_legend_location_count) {
            throw std::invalid_argument("invalid legend location");
        }
        m_legend[where].reset();
        m_layout_dirty = true;
        m_need_redraw = true;
        ++m_layout_generation;
    }

    axis& get_axis(axis_e axis_dir)
    {
        return (axis_dir == x_axis ? m_x_axis : m_y_axis);
    }

    const axis& get_axis(axis_e axis_dir) const
    {
        return (axis_dir == x_axis ? m_x_axis : m_y_axis);
    }

    void set_axis_labels_angle(axis_e axis, double angle);

    double get_axis_labels_angle(axis_e axis_dir) const
    {
        return get_axis(axis_dir).labels_angle();
    }

    void set_units(bool use_units);

    bool use_units() const {
        return m_x_axis.active;
    };

    void update_units();
    void set_limits(const agg::rect_d& r);
    void unset_limits();

    void set_xaxis_comp_labels(ptr_list<factor_labels>* labels)
    {
        m_x_axis.set_comp_labels(labels);
    }

    void add(std::unique_ptr<sg_drawable> element);

    void add(sg_element element) {
        add(std::make_unique<sg_element>(std::move(element)));
    }

    void add(elem_object *object, agg::rgba8 stroke_color, float stroke_width, agg::rgba8 fill_color, unsigned flags = graphics::property::fill|graphics::property::stroke) {
        add(sg_element{object, stroke_color, stroke_width, fill_color, flags});
    }

    void before_draw();

    void get_bounding_rect(agg::rect_base<double>& bb)
    {
        before_draw();

        if (m_rect.is_defined())
            bb = m_rect.rect();
        else
            bb = agg::rect_base<double>(0.0, 0.0, 0.0, 0.0);
    }

    bool push_layer();
    bool pop_layer();
    void clear_current_layer();

    /* drawing queue related methods */
    void push_drawing_queue();
    void clear_drawing_queue();
    int current_layer_index();

    bool clip_is_active() const {
        return m_clip_flag;
    };
    void set_clip_mode(bool flag) {
        m_clip_flag = flag;
    };

    bool need_redraw() const {
        return m_need_redraw;
    };

    unsigned layout_generation() const {
        return m_layout_generation;
    }
    void commit_pending_draw();

    void pad_mode(bool req)
    {
        axis& ax = m_x_axis;
        if (req != ax.pad_units)
        {
            ax.pad_units = req;
            m_need_redraw = true;
            compute_user_trans();
        }
    };

    bool pad_mode() const {
        return m_x_axis.pad_units;
    }

    bool enable_label_format(axis_e dir, const char* fmt)
    {
        if (!fmt)
        {
            get_axis(dir).clear_label_format();
            return true;
        }

        units::format_e tag = units::parse_label_format(fmt);
        if (tag == units::format_invalid)
            return false;
        get_axis(dir).set_label_format(tag, fmt);
        return true;
    }

    void enable_categories(axis_e dir) {
        get_axis(dir).use_categories = true;
    }

    void disable_categories(axis_e dir)
    {
        axis& ax = get_axis(dir);
        ax.use_categories = false;
        ax.categories.clear();
    }

    void add_category_entry(axis_e dir, double v, const char* text)
    {
        axis& ax = get_axis(dir);
        ax.categories.add_item(v, text);
    }

    template <class Canvas>
    void draw_queue(drawing_context& dc, Canvas& canvas, const agg::trans_affine& m, const plot_render_info& inf, opt_rect<double>& bbox);

    template <class Canvas>
    void draw(drawing_context& dc, Canvas& canvas, const agg::trans_affine& m, plot_render_info* inf)
    {
        canvas_adapter<Canvas> vc(&canvas);
        agg::rect_i clip = rect_of_slot_matrix<int>(m);
        plot_layout layout = compute_plot_layout(m);
        draw_virtual_canvas(vc, layout, &clip);
        m_need_redraw = false;
        m_layout_dirty = false;
        if (inf)
        {
            inf->active_area = layout.plot_active_area;
            inf->layout_generation = m_layout_generation;
        }
    }

    template <class Canvas>
    void draw(drawing_context& dc, Canvas& canvas, const agg::rect_i& r, plot_render_info* inf)
    {
        canvas_adapter<Canvas> vc(&canvas);
        agg::trans_affine mtx = affine_matrix(r);
        plot_layout layout = compute_plot_layout(mtx);
        draw_virtual_canvas(vc, layout, &r);
        m_need_redraw = false;
        m_layout_dirty = false;
        if (inf)
        {
            inf->active_area = layout.plot_active_area;
            inf->layout_generation = m_layout_generation;
        }
    }

private:
    void draw_virtual_canvas(canvas_type& canvas, plot_layout& layout, const agg::rect_i* r);

    void draw_grid(const axis_e dir, const units& u,
                   const agg::trans_affine& user_mtx,
                   agg::path_storage& ln);

    void draw_elements(canvas_type &canvas, const plot_layout& layout);

    void draw_axis(canvas_type& can, plot_layout& layout, const agg::rect_i* clip = 0);

    void draw_title(canvas_type& canvas, const plot_layout& layout);
    void draw_outside_legends(canvas_type& canvas, const plot_layout& layout);
    void draw_inside_legends(canvas_type& canvas, const plot_layout& layout,
                             opt_rect<double>* painted_bounds = nullptr);

    plot_layout compute_plot_layout(const agg::trans_affine& canvas_mtx, bool do_legends = true);

    // return the matrix that map from plot coordinates to screen
    // coordinates
    agg::trans_affine get_model_matrix(const plot_layout& layout);

    void clip_plot_area(canvas_type& canvas, const agg::trans_affine& canvas_mtx);

    void compute_user_trans();

    bool fit_inside(const sg_drawable& elem) const;
    void check_bounding_box();
    void calc_layer_bounding_box(item_list* layer, opt_rect<double>& rect);
    void calc_bounding_box();
    void set_opt_limits(const opt_rect<double>& r);

    void set_string_property(str& property, const char *new_value) {
        if (strcmp(property.cstr(), new_value) != 0) {
            property = new_value;
            m_need_redraw = true;
        }
    }

    unsigned nb_layers() const {
        return m_layers.size();
    }
    item_list* get_layer(unsigned j) {
        return &m_layers[j];
    }

    item_list* current_layer() {
        return &m_layers.back();
    };

    item_list* parent_layer()
    {
        unsigned n = m_layers.size();
        return (n > 1 ? &m_layers[n-2] : nullptr);
    }

    agg::trans_affine m_trans;
    sg_drawable_list m_drawing_queue;

    bool m_clip_flag;

    bool m_need_redraw;
    opt_rect<double> m_rect;

    // keep trace of the region where changes happened since
    // the last pushlayer or clear
    opt_rect<double> m_changes_accu;
    opt_rect<double> m_changes_pending;
private:
    std::vector<item_list> m_layers;

    str m_title;

    axis m_x_axis, m_y_axis;
    std::unique_ptr<legend> m_legend[plot_legend_location_count];

    bool m_auto_limits;
    bool m_bbox_updated;
    bool m_enlarged_layer;
    bool m_layout_dirty;
    unsigned m_layout_generation = 0;
    std::mutex m_drawing_mutex;
};

template <class Canvas>
void plot::draw_queue(plot::drawing_context& dc, Canvas& _canvas, const agg::trans_affine& canvas_mtx, const plot_render_info& inf, opt_rect<double>& bb)
{
    canvas_adapter<Canvas> canvas(&_canvas);
    before_draw();

    plot_layout layout = compute_plot_layout(canvas_mtx);
    layout.plot_active_area = inf.active_area;

    clip_plot_area(canvas, layout.plot_active_area);

    const bool repaint_inside_legends =
        m_drawing_queue.size() != 0 || m_changes_pending.is_defined();
    const agg::trans_affine m = get_model_matrix(layout);
    for (auto& d : m_drawing_queue)
    {
        opt_rect<double> painted_bounds;
        d->draw(canvas, m, &painted_bounds);
        bb.add<rect_union>(painted_bounds);
    }
    if (m_changes_pending.is_defined()) {
        bb.add<rect_union>(m_changes_pending);
    }
    canvas.reset_clipping();

    if (repaint_inside_legends) {
        opt_rect<double> inside_bounds;
        draw_inside_legends(canvas, layout, &inside_bounds);
        bb.add<rect_union>(inside_bounds);
    }
    m_changes_accu.add<rect_union>(bb);
}

} /* namespace graphics */
