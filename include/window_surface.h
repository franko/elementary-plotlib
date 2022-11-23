#pragma once

#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <agg_trans_affine.h>

#include "display_window.h"
#include "elem_plot.h"
#include "image_buf.h"
#include "window_part.h"
#include "canvas.h"
#include "rect.h"
#include "plot.h"

namespace graphics {

// Note: the window_surface does not need to have point to an elem_plot
// but we may give an interface that implements only the methods draw()
// and draw_queue().
// The benefits would be a better decoupling between the classes and
// the possibility to give to elem_plot something else than a elem_plot
// generalizing in this way the things that window_surface can render.
struct plot_ref {
    plot_ref(): plot_ptr(nullptr), have_save_img(false), pending_queue(false) {}

    elem_plot* plot_ptr;
    plot_render_info inf;
    bool have_save_img;
    // If true we are between an update send_request() and a commit_pending_draw():
    // render the drawing_queue() on render().
    bool pending_queue;
};

// The class window_surface is part of a window. It is declared by window_gen
//   and the screen window implementation gets a reference to the window_surface.
//
//   The window, including the window surface, run on its own thread. In order
//   to ensure memory safety and avoid race conditions the main thread and
//   the window's thread should interact only using some predefined methods.
//
//   The window contains references to the main thread with the pointers to
//   plots. The interaction of the window's with the plots should be strictly
//   limited.
//
//   The principles of the operations are:
//
//   1. the plots referenced from window_surface are rendered on the
//      windows buffer only from the window's thread.
//   2. the paint operations are all done from the window's thread
//   3. when the plots needs to update the window it sends to the
//      window's thread a request by communicating the plot's index
//      relative to the window surface.
//      In response to the request the window's thread will render the
//      plot and paint on the screen (see point 1).
//
//   The window screen implementation (window_win32 or xwindow) calls
//   the following window_surface's methods:
//
//   resize, render, slot_refresh, update_window_area.
//
//   From the plot_agent the following methods are called:
//
//   slot_refresh_request, clear_pending_flags.
class window_surface {
public:
    window_surface(const char* split);
    ~window_surface();

    /* Methods for initial window's setup. */
    void attach_window(display_window* win) { m_window = win; }
    int attach(elem_plot* p, const char* slot_str);
    void split(const char* split_str);

    /* The following methods are called from the main thread. */
    void slot_refresh_request(unsigned index);
    void clear_pending_flags(int plot_index);

    /* The following method can be called only from the window's thread
       and will lock the plot. */
    bool resize(unsigned ww, unsigned hh, int w_pixels, int h_pixels);
    void render();
    void slot_refresh(unsigned index);
    void update_window_area();

    agg::pod_bvector<elem_plot *> linked_plots() const {
        agg::pod_bvector<elem_plot *> linked_plots_list;
        for (unsigned i = 0; i < plot_number(); i++) {
            auto plot_at_index = get_plot(i);
            if (plot_at_index) {
                linked_plots_list.add(plot_at_index);
            }
        }
        return linked_plots_list;
    }

private:
    void render_by_ref(plot::drawing_context& dc, plot_ref& ref, const agg::rect_i& r);
    void render_plot_by_index(plot::drawing_context& dc, unsigned index);
    opt_rect<int> render_drawing_queue(plot::drawing_context& dc, unsigned index);
    opt_rect<int> render_drawing_queue(plot::drawing_context& dc, plot_ref& ref, const agg::rect_i& r);

    elem_plot* get_plot(unsigned index) const { return m_plots[index].plot_ptr; }

    agg::rect_i get_plot_area(unsigned index) const;
    unsigned plot_number() const { return m_plots.size(); }

    bool save_plot_image(unsigned index);
    bool restore_plot_image(unsigned index);
    void save_slot_image(unsigned index);
    void restore_slot_image(unsigned index);

    int get_width()  const { return m_img.width(); }
    int get_height() const { return m_img.height(); }

    image m_img;
    image m_save_img;
    window_part m_part;
    agg::pod_bvector<plot_ref> m_plots;
    display_window* m_window;
    canvas* m_canvas;
};

} /* namespace graphics */
