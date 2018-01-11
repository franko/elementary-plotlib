#ifndef FOXGUI_WINDOW_SURFACE_H
#define FOXGUI_WINDOW_SURFACE_H

#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <agg_trans_affine.h>

#include "image_buf.h"
#include "window_part.h"
#include "sg_object.h"
#include "canvas.h"
#include "rect.h"
#include "plot.h"

namespace graphics {

typedef image_gen<pixel_size, flip_y> image;

struct display_window {
    virtual void update_region(image& img, const agg::rect_i& r) = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual ~display_window() { };
};

struct plot_ref {
    plot_ref(): plot_ptr(NULL), have_save_img(false) {}

    plot* plot_ptr;
    plot_render_info inf;
    bool have_save_img;
};

class window_surface
{
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

    // redraw all the image buffer for the current plots
    void draw_image_buffer();

    int get_width()  const { return m_img.width(); }
    int get_height() const { return m_img.height(); }

    void render(unsigned index);
    opt_rect<int> render_drawing_queue(unsigned index);

    void render_all();
    void draw_request();

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

private:
    static void drawing_lock()   { drawing_mutex.lock(); }
    static void drawing_unlock() { drawing_mutex.unlock(); }

    void update_region_locked(image& img, const agg::rect_i& r);
    void render(plot_ref& ref, const agg::rect_i& r);
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

#endif
