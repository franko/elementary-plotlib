#pragma once

#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <agg_trans_affine.h>
#include <mutex>

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

/* The class window_surface is part of a window. It is declared by window_gen
   and the actual window implementation gets a reference to the window_surface.
   The plots, in turns, can interact with the window_surface by
   
   - drawing on the window_surface image buffer
   - request to draw the updated image to the window.

    As the plots and the windows run in separate threads it is required to
    protect the access to the images member variables. The access to the
    canvas pointer should be protected too because it points to the underlying
    image.

    To protect the concurrent access to the image and canvas member variables
    a special canvas is used.

    The window_surface::image_guard is used to protect calls to the method
    get_image().

    From the window's implementation the following methods are called:

    resize, render, get_image.

    From the plot_agent the following methods are called:

    slot_refresh.
*/
class window_surface {
public:
    class image_guard {
    public:
        image_guard(window_surface& ws): m_window_surface(ws) {
            m_window_surface.m_image_mutex.lock();
        }
        ~image_guard() {
            m_window_surface.m_image_mutex.unlock();
        }
    private:
        window_surface& m_window_surface;
    };

    window_surface(const char* split);
    ~window_surface();

    void attach_window(display_window* win) { m_window = win; }
    int attach(plot* p, const char* slot_str);
    void split(const char* split_str);
    bool resize(unsigned ww, unsigned hh);
    void render();
    void slot_refresh(unsigned index);
    const image& get_image(image_guard& guard) { return m_img; }

private:
    void render_plot_by_index(unsigned index);
    opt_rect<int> render_drawing_queue(unsigned index);

    plot* get_plot(unsigned index) const { return m_plots[index].plot_ptr; }

    agg::rect_i get_plot_area(unsigned index) const;
    unsigned plot_number() const { return m_plots.size(); }

    bool save_plot_image(unsigned index);
    bool restore_plot_image(unsigned index);
    void save_slot_image(unsigned index);
    void restore_slot_image(unsigned index);

    void render_by_ref_unprotected(plot_ref& ref, const agg::rect_i& r);
    opt_rect<int> render_drawing_queue(plot_ref& ref, const agg::rect_i& r);
    opt_rect<int> render_drawing_queue_unprotected(plot_ref& ref, const agg::rect_i& r);

    int get_width()  const { return m_img.width(); }
    int get_height() const { return m_img.height(); }

    image m_img;
    image m_save_img;
    window_part m_part;
    agg::pod_bvector<plot_ref> m_plots;
    display_window* m_window;
    canvas* m_canvas;
    std::mutex m_image_mutex;
};

} /* namespace graphics */
