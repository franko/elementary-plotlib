#include "window_fox_kernel.h"
#include "debug_priv.h"

window_fox_kernel::window_fox_kernel():
        m_drawable(nullptr), m_update_signal(nullptr), m_start_signal(nullptr) {
}

window_fox_kernel::~window_fox_kernel() {
    delete m_update_signal;
    delete m_start_signal;
}

void window_fox_kernel::bind_drawable(FXDrawable *drawable, FXSelector update_selector) {
    m_drawable = drawable;
    m_update_signal = new FXGUISignal(app(), m_drawable, update_selector, nullptr);
}


FXApp *window_fox_kernel::app() {
    return (m_drawable ? m_drawable->getApp() : nullptr);
}

void window_fox_kernel::update_region(graphics::image& src_img, const agg::rect_i& r) {
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

    FXDCWindow dc(m_drawable);
    dc.drawImage(&img, r.x1, m_drawable->getHeight() - r.y2);
}

void window_fox_kernel::update_region_request(graphics::image& img, const agg::rect_i& r) {
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
        m_update_signal->signal();
        m_update_notify.wait();
        m_update_region.clear();
    }
}

void window_fox_kernel::bind_window_environment(FXObject *env_object, FXSelector start_selector) {
    if (! m_drawable) {
        debug_log(0, "error: calling bind_window_environement without a drawable");
        return;
    }
    m_start_signal = new FXGUISignal(app(), env_object, start_selector, this);
}

void window_fox_kernel::start(unsigned width, unsigned height, unsigned flags) {
    if (! m_start_signal) {
        debug_log(0, "error: cannot start fox window, no hosting environment");
        return;
    }
    m_start_signal->signal();
    request_error_e status = wait_until_notification(graphics::window_running);
    if (!(status == request_satisfied || status == request_success)) {
        debug_log(1, "error starting window, return code: %d", int(status));
    }
}
