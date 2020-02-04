#pragma once

#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <agg_trans_affine.h>

#include "display_window.h"
#include "image_buf.h"
#include "window_part.h"
#include "canvas.h"
#include "rect.h"
#include "plot.h"

namespace graphics {

struct plot_ref {
    plot_ref(): plot_ptr(NULL), have_save_img(false) {}

    plot* plot_ptr;
    plot_render_info inf;
    bool have_save_img;
};

class window_surface {
public:
    window_surface(const char* split);
    ~window_surface();

    int attach(plot* p, const char* slot_str);
    void split(const char* split_str);

    void attach_window(display_window* win) { m_window = win; }

    bool canvas_size_match(unsigned ww, unsigned hh)
    {
        return (m_img.width() == ww && m_img.height() == hh);
    }

    bool resize(unsigned ww, unsigned hh);
    void render();

    // redraw all the image buffer for the current plots
    void draw_image_buffer();

    int get_width()  const { return m_img.width(); }
    int get_height() const { return m_img.height(); }

    void render_plot_by_index(unsigned index);
    opt_rect<int> render_drawing_queue(unsigned index);

    plot* get_plot(unsigned index) const { return m_plots[index].plot_ptr; }

    agg::rect_i get_plot_area(unsigned index) const;
    agg::rect_i get_plot_area(unsigned index, int width, int height) const;
    unsigned plot_number() const { return m_plots.size(); }

    bool is_ready() const { return (m_canvas != 0); }

    bool save_plot_image(unsigned index);
    bool restore_plot_image(unsigned index);

    const image& get_image() { return m_img; }

    void slot_refresh(unsigned index);
    void slot_update(unsigned index);
    void save_slot_image(unsigned index);
    void restore_slot_image(unsigned index);

    void update_region_request(image& img, const agg::rect_i& r) {
        m_window->update_region_request(img, r);
    }

private:
    void render_by_ref(plot_ref& ref, const agg::rect_i& r);
    opt_rect<int> render_drawing_queue(plot_ref& ref, const agg::rect_i& r);

    bool plot_is_defined(unsigned index) const
    {
        return (m_plots[index].plot_ptr != NULL);
    }

    image m_img;
    image m_save_img;
    window_part m_part;
    agg::pod_bvector<plot_ref> m_plots;
    display_window* m_window;
    canvas* m_canvas;
};

} /* namespace graphics */
