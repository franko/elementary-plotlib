#include "window_surface.h"
#include "window_flags.h"

#include "util/agg_color_conv_rgb8.h"

#include "debug_priv.h"
#include "fatal.h"
#include "colors.h"

namespace graphics {

window_surface::window_surface(const char* split_str):
m_img(), m_save_img(), m_window(nullptr), m_canvas(nullptr)
{
    split(split_str ? split_str : ".");
}

window_surface::~window_surface()
{
    delete m_canvas;
}

void window_surface::split(const char* split_str)
{
    m_part.parse(split_str);
    m_part.split();

    m_plots.clear();
    plot_ref empty;
    for (unsigned k = 0; k < m_part.get_slot_number(); k++)
        m_plots.add(empty);
}

bool window_surface::resize(unsigned ww, unsigned hh, int w_pixels, int h_pixels)
{
    m_save_img.clear();

    for (unsigned k = 0; k < plot_number(); k++)
        m_plots[k].have_save_img = false;

    if (likely(m_img.resize(w_pixels, h_pixels)))
    {
        delete m_canvas;
        m_canvas = new(std::nothrow) canvas(m_img, w_pixels / ww, h_pixels / hh, colors::white());
        return (m_canvas != NULL);
    }
    return false;
}

void window_surface::render_by_ref(plot::drawing_context& dc, plot_ref& ref, const agg::rect_i& r)
{
    m_canvas->clear_logical_area(r);
    if (ref.plot_ptr) {
        ref.plot_ptr->draw(dc, *m_canvas, r, &ref.inf);
    }
}

void window_surface::render_plot_by_index(plot::drawing_context& dc, unsigned index)
{
    int canvas_width, canvas_height;
    get_canvas_logical_size(canvas_width, canvas_height);
    plot_ref& ref = m_plots[index];
    agg::rect_i area = m_part.rect(index, canvas_width, canvas_height);
    render_by_ref(dc, ref, area);
}

opt_rect<int>
window_surface::render_drawing_queue(plot::drawing_context& dc, plot_ref& ref, const agg::rect_i& box)
{
    const agg::trans_affine m = affine_matrix(box);
    opt_rect<double> r;
    ref.plot_ptr->draw_queue(dc, *m_canvas, m, ref.inf, r);
    opt_rect<int> ri;
    if (r.is_defined())
    {
        const agg::rect_d& rx = r.rect();
        ri.set(rx.x1, rx.y1, rx.x2, rx.y2);
    }
    return ri;
}

opt_rect<int>
window_surface::render_drawing_queue(plot::drawing_context& dc, unsigned index)
{
    int canvas_width, canvas_height;
    get_canvas_logical_size(canvas_width, canvas_height);
    plot_ref& ref = m_plots[index];

    if (unlikely(!ref.plot_ptr))
        fatal_exception("call to plot_draw_queue for undefined plot");

    agg::rect_i area = m_part.rect(index, canvas_width, canvas_height);
    return render_drawing_queue(dc, ref, area);
}

int window_surface::attach(elem_plot* p, const char* slot_str)
{
    int index = m_part.get_slot_index(slot_str);
    if (index >= 0)
        m_plots[index].plot_ptr = p;
    return index;
}

bool window_surface::save_plot_image(unsigned index)
{
    int ww, hh;
    get_image_pixels_size(ww, hh);
    if (unlikely(!m_save_img.ensure_size(ww, hh))) return false;

    agg::rect_i r = m_part.rect(index, ww, hh);
    image::copy_region(m_save_img, m_img, r);
    m_plots[index].have_save_img = true;
    return true;
}

bool window_surface::restore_plot_image(unsigned index)
{
    if (unlikely(!m_plots[index].have_save_img))
        fatal_exception("window_surface::restore_slot_image invalid restore image");

    int ww, hh;
    get_image_pixels_size(ww, hh);
    agg::rect_i r = m_part.rect(index, ww, hh);
    image::copy_region(m_img, m_save_img, r);
    return true;
}

agg::rect_i window_surface::get_plot_pixels_area(unsigned index) const
{
    int w_pixels, h_pixels;
    get_image_pixels_size(w_pixels, h_pixels);
    return m_part.rect(index, w_pixels, h_pixels);
}

void window_surface::slot_refresh_request(unsigned index) {
    bool request_success = m_window->send_request(graphics::window_request::update, index);
    if (!request_success) {
        debug_log(1, "window_surface, update request failed");
    }
    m_plots[index].pending_queue = true;
}

void window_surface::update_window_area() {
    int w_pixels, h_pixels;
    get_image_pixels_size(w_pixels, h_pixels);
    const agg::rect_i r(0, 0, w_pixels, h_pixels);
    m_window->update_region(m_img, r);
}

// Add an argument to indicate if only a render_drawing_queue
// is needed.
void window_surface::slot_refresh(unsigned index)
{
    if (m_canvas == nullptr) return;
    elem_plot *plot_selected = get_plot(index);
    plot::drawing_context dc(*plot_selected);
    bool redraw = get_plot(index)->need_redraw();
    if (redraw) {
        render_plot_by_index(dc, index);
        render_drawing_queue(dc, index);
        agg::rect_i area = get_plot_pixels_area(index);
        m_window->update_region(m_img, area);
    } else {
        opt_rect<int> r = render_drawing_queue(dc, index);
        agg::rect_i area = get_plot_pixels_area(index);
        if (r.is_defined()) {
            const int pad = 4;
            const agg::rect_i& ri = r.rect();
            agg::rect_i r_pad(ri.x1 - pad, ri.y1 - pad, ri.x2 + pad, ri.y2 + pad);
            r_pad.clip(area);
            m_window->update_region(m_img, m_canvas->hidpi_scaled_rect(r_pad));
        }
    }
}

void
window_surface::render()
{
    for (unsigned k = 0; k < m_plots.size(); k++) {
        elem_plot *current_plot = get_plot(k);
        if (current_plot) {
            plot::drawing_context dc(*current_plot);
            render_plot_by_index(dc, k);
            if (m_plots[k].pending_queue) {
                render_drawing_queue(dc, k);
            }
        }
    }
}

void
window_surface::save_slot_image(unsigned index)
{
    save_plot_image(index);
}

void
window_surface::restore_slot_image(unsigned index)
{
    if (m_plots[index].have_save_img)
    {
        restore_plot_image(index);
    }
    else
    {
        elem_plot *current_plot = get_plot(index);
        if (current_plot) {
            plot::drawing_context dc(*current_plot);
            render_plot_by_index(dc, index);
            save_plot_image(index);
        }
    }
}

void window_surface::clear_pending_flags(int plot_index) {
    m_plots[plot_index].pending_queue = false;
}

} /* namespace graphics */
