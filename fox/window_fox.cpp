#include "window_fox.h"
#include "FXLibcanvasWindow.h"
#include "debug_priv.h"

window_fox::window_fox(FXElpWindow *canvas, const char *split_str) : m_plot_canvas(canvas), m_surface(split_str)
{
    m_surface.attach_window(this);
    m_gui_signal = new FXGUISignal(app(), m_plot_canvas, FXElpWindow::ID_UPDATE_REGION, nullptr);
}

window_fox::~window_fox() {
    delete m_gui_signal;
}

FXApp *window_fox::app() {
    return m_plot_canvas->getApp();
}

void window_fox::update_region(graphics::image& src_img, const agg::rect_i& r) {
    const unsigned fximage_pixel_size = 4; // 32 bit RGBA format.
    const bool fximage_flipy = true;
    const int width = r.x2 - r.x1, height = r.y2 - r.y1;

    if (width <= 0 || height <= 0) return;

    debug_log(1, "update_region rect: %d %d %d %d", r.x1, r.y1, r.x2, r.y2);

    /* Create a rendering_buffer and the underlying memory buffer to
       store a FXColor array. */
    image_gen<fximage_pixel_size, fximage_flipy> fxcolor_image{unsigned(width), unsigned(height)};

    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    rendering_buffer_copy(fxcolor_image, agg::pix_format_rgba32, src_view, (agg::pix_format_e) graphics::pixel_format);

    FXColor *fxcolor_buf = (FXColor *) fxcolor_image.buf();
    FXImage img(app(), fxcolor_buf, IMAGE_KEEP, width, height);
    img.create();

    FXDCWindow dc(m_plot_canvas);
    dc.drawImage(&img, r.x1, m_plot_canvas->getHeight() - r.y2);
}

void window_fox::update_region_request(graphics::image& img, const agg::rect_i& r) {
    if (std::this_thread::get_id() == m_window_thread_id) {
        // We are running in the thread of the Window's event loop. Just do the
        // drawing operation.
        debug_log(1, "update_region request from window's thread");
        update_region(img, r);
    } else {
        // We are on another thread. Interrupt the Window's thread to request
        // the update_region operation.
        debug_log(1, "update_region request from secondary thread");
        m_update_notify.prepare();
        m_update_region.prepare(img, r);
        m_gui_signal->signal();
        m_update_notify.wait();
        m_update_region.clear();
    }
}
