#include "win32/window_win32.h"

template<class RenBufDst, class RenBufSrc, class CopyRow>
void my_color_conv(RenBufDst* dst, const RenBufSrc* src, CopyRow copy_row_functor)
{
    unsigned int width  = src->width();
    unsigned int height = src->height();

    for(unsigned int y = 0; y < height; y++)
    {
        copy_row_functor(dst->row_ptr(0, y, width), src->row_ptr(y), width);
    }
}

//------------------------------------------------------------------------
HINSTANCE window_win32::g_windows_instance = 0;
int       window_win32::g_windows_cmd_show = 0;

window_win32::window_win32() :
    m_sys_format(graphics::pixel_format),
    m_sys_bpp(gslshell::sys_bpp),
    m_hwnd(0),
    m_bmp_draw(0),
    m_cur_x(0),
    m_cur_y(0),
    m_input_flags(0),
    m_redraw_flag(true),
    m_current_dc(0),
    m_is_mapped(false),
    m_is_ready(false)
{
}

window_win32::~window_win32() {
    if (m_bmp_draw) {
        delete [] (unsigned char*) m_bmp_draw;
    }
}

void window_win32::close() {
    ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
}

void window_win32::create_pmap(unsigned width, unsigned height, agg::rendering_buffer* wnd)
{
    m_pmap_window.create(width, height, org_e(target_bpp));
    pixel_map_attach (m_pmap_window, wnd, target_flip_y);

    if (m_bmp_draw) {
        delete [] (unsigned char*) m_bmp_draw;
    }
    m_bmp_draw = pixel_map::create_bitmap_info(width, height, org_e(m_sys_bpp));
}

//------------------------------------------------------------------------
void window_win32::display_pmap(HDC dc, const agg::rendering_buffer* src, const agg::rect_base<int> *ri)
{
    if(m_sys_format == m_format && ri == nullptr)
    {
        m_pmap_window.draw(dc);
    }
    else
    {
        agg::rect_base<int> r(0, 0, src->width(), src->height());
        if (ri)
            r = agg::intersect_rectangles(r, *ri);

        int w = r.x2 - r.x1, h = r.y2 - r.y1;

        bitmap_info_resize (m_bmp_draw, w, h);

        pixel_map pmap;
        pmap.attach_to_bmp(m_bmp_draw);

        rendering_buffer rbuf_tmp;
        pixel_map_attach (pmap, &rbuf_tmp, m_flip_y);

        rendering_buffer_ro src_view;
        rendering_buffer_get_const_view(src_view, *src, r, m_bpp / 8);

        if (m_sys_format == graphics::pixel_format)
        {
            rbuf_tmp.copy_from(src_view);
        }
        else
        {
            if (m_sys_format == pix_format_bgr24 && graphics::pixel_format == pix_format_rgb24)
            {
                my_color_conv(&rbuf_tmp, &src_view, color_conv_rgb24_to_bgr24());
            }
        }

        unsigned int wh = m_pmap_window.height();
        RECT wrect;
        wrect.left   = r.x1;
        wrect.right  = r.x2;
        wrect.bottom = wh - r.y1;
        wrect.top    = wh - r.y2;

        RECT brect;
        brect.left   = 0;
        brect.right  = w;
        brect.bottom = h;
        brect.top    = 0;

        pmap.draw(dc, &wrect, &brect);
    }
}
