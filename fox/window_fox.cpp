#include "fox/window_fox.h"
#include "fox/GraphicsWindow.h"
#include "debug_log.h"

window_fox::window_fox(graphics::render_target& tgt, GraphicsWindow *canvas):
    m_plot_canvas(canvas), m_target(tgt)
{
    m_plot_canvas->bind(this);
    m_gui_signal = new FXGUISignal(app(), m_plot_canvas, GraphicsWindow::ID_UPDATE_REGION, nullptr);
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

    debug_log("update_region: %d %d", width, height);

    /* Create a rendering_buffer and the underlying memory buffer to
       store a FXColor array. */
    image_gen<fximage_pixel_size, fximage_flipy> fxcolor_image{width, height};

    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    rendering_buffer_copy(fxcolor_image, agg::pix_format_rgba32, src_view, (agg::pix_format_e) graphics::pixel_format);

    FXColor *fxcolor_buf = (FXColor *) fxcolor_image.buf();
    FXImage img(app(), fxcolor_buf, IMAGE_KEEP, width, height);
    img.create();

    FXDCWindow dc(m_plot_canvas);
    dc.drawImage(&img, r.x1, r.y1);
}

void window_fox::update_region_request(graphics::image& img, const agg::rect_i& r) {
    m_update_region.prepare(img, r);
    m_gui_signal->signal();
    m_update_notify.wait();
    m_update_region.clear();
}
