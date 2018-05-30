#include "fox/window_fox.h"
#include "fox/PlotCanvas.h"

window_fox::window_fox(graphics::render_target& tgt, FXCanvas *canvas):
    m_fx_canvas(canvas), m_target(tgt)
{
}

void window_fox::start(unsigned width, unsigned height, unsigned flags) {
#if 0
    const char *dummy_argv[1] = {"app"};
    m_window_status.set(graphics::window_starting);
    FXApp app("libcanvas", "libcanvas");
    app.init(1, dummy_argv);
#endif
    main_window = new FXMainWindow(&g_app, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    m_canvas = new PlotCanvas(m_main_window, nullptr, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN);
    app.create();
    m_window_status.set(graphics::window_running);
    app.run();
    m_window_status.set(graphics::window_closed);
    m_fx_canvas = nullptr;
    // m_main_window = nullptr;
}

void window_fox::update_region(graphics::image& src_img, const agg::rect_i& r) {
    const unsigned fximage_pixel_size = 4; // 32 bit RGBA format.
    const bool fximage_flipy = true;
    const int width = r.x2 - r.x1, height = r.y2 - r.y1;

    /* Create a rendering_buffer and the underlying memory buffer to
       store a FXColor array. */
    image_gen<fximage_pixel_size, fximage_flipy> fxcolor_image{width, height};

    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    rendering_buffer_copy(fxcolor_image, agg::pix_format_rgba32, src_view, (agg::pix_format_e) graphics::pixel_format);

    FXColor *fxcolor_buf = (FXColor *) fxcolor_image.buf();
    FXImage img(getApp(), fxcolor_buf, 0, width, height);

    FXDCWindow dc(m_canvas);
    dc.drawImage(&img, r.x1, r.y1);
}
