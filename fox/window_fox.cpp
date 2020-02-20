#include "window_fox.h"
#include "debug_priv.h"
#include "FXElemBaseWindow.h"

window_fox::window_fox(graphics::window_surface& window_surface):
        m_drawable(nullptr),
        m_update_signal(nullptr),
        m_start_signal(nullptr),
        m_window_surface(window_surface) {
}

window_fox::window_fox(graphics::window_surface& window_surface, FXGUISignal *start_signal):
        window_fox(window_surface) {
    m_start_signal = start_signal;
}

window_fox::window_fox(graphics::window_surface& window_surface, FXElemBaseWindow *elem_window):
        window_fox(window_surface) {
    elem_window->setWindowFox(this);
    bind_drawable(elem_window, FXElemBaseWindow::ID_ELEM_UPDATE, FXElemBaseWindow::ID_ELEM_CLOSE);
}

window_fox::~window_fox() {
    delete m_update_signal;
    // Do not delete m_start_signal since we are not the owner.
}

void window_fox::bind_drawable(FXDrawable *drawable, FXSelector update_selector, FXSelector close_selector) {
    m_drawable = drawable;
    m_update_signal = new FXGUISignal(drawable->getApp(), m_drawable, update_selector, nullptr);
    m_close_signal = new FXGUISignal(drawable->getApp(), m_drawable, close_selector, nullptr);
}

void window_fox::update_region(const graphics::image& src_img, const agg::rect_i& r) {
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

    FXApp *app = m_drawable->getApp();
    FXColor *fxcolor_buf = (FXColor *) fxcolor_image.buf();
    FXImage img(app, fxcolor_buf, IMAGE_KEEP, width, height);
    img.create();

    FXDCWindow dc(m_drawable);
    dc.drawImage(&img, r.x1, m_drawable->getHeight() - r.y2);
}

bool window_fox::send_request(graphics::window_request request_type, int index) {
    if (std::this_thread::get_id() == m_window_thread_id) {
        switch (request_type) {
        case graphics::window_request::update:
            // We are running in the thread of the Window's event loop. Just do the
            // drawing operation.
            debug_log(1, "update_region request from window's thread");
            m_window_surface.slot_refresh(index);
            break;
        case graphics::window_request::close:
            delete m_drawable;
        }
    } else {
        switch (request_type) {
        case graphics::window_request::update:
            // We are on another thread. Interrupt the Window's thread to request
            // the update_region operation.
            debug_log(1, "update_region request from secondary thread");
            m_update_notify.start(index);
            m_update_signal->signal();
            m_update_notify.wait();
            break;
        case graphics::window_request::close:
            m_close_signal->signal();
            wait_for_status(graphics::window_closed);
            break;
        }
    }
    return true;
}

void window_fox::start(unsigned width, unsigned height, unsigned flags) {
    FXElemStartMessage data{this, width, height, flags};
    if (! m_start_signal) {
        debug_log(0, "error: cannot start fox window, no hosting environment");
        return;
    }
    m_start_signal->setData(&data);
    m_start_signal->signal();
    wait_for_status(graphics::window_running);
    m_start_signal->setData(nullptr);
}
