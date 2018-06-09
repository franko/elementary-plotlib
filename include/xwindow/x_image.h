#pragma once

#include <new>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "render_config.h"
#include "agg_rendering_buffer.h"

class x_image : public agg::rendering_buffer {
    enum { flip_y = graphics::flip_y };

public:
    x_image(unsigned pixel_size, unsigned byte_order,
            unsigned w, unsigned h, x_connection *xc = 0) :
        m_pixel_size(pixel_size)
    {
        init(w, h);

        unsigned bpp = pixel_size * sizeof(char);
        unsigned row_size = pixel_size * w;

        if (xc)
        {
            m_img = XCreateImage(xc->display, xc->visual, xc->depth,
                                 ZPixmap, 0, (char*) buf(),
                                 width(), height(), bpp, row_size);
            m_img->byte_order = byte_order;
        }
        else
        {
            m_img = 0;
        }
    }

    ~x_image()
    {
        if (m_img)
        {
            m_img->data = 0;
            XDestroyImage(m_img);
        }

        dispose();
    };

    void x_resize(unsigned w, unsigned h)
    {
        /* The following resize the rendering buffer using the same
           buffer memory. We assume that the size can only shrink. */
        unsigned row_size = w * m_pixel_size;
        attach(buf(), w, h, flip_y ? -row_size : row_size);

        m_img->width  = w;
        m_img->height = h;
        m_img->bytes_per_line = row_size;
    }

    XImage* ximage() {
        return m_img;
    };

private:
    bool init(unsigned w, unsigned h)
    {
        agg::int8u* data = new(std::nothrow) agg::int8u[w * h * m_pixel_size];
        if (likely(data))
        {
            int stride = (flip_y ? - w * m_pixel_size : w * m_pixel_size);
            attach(data, w, h, stride);
        }
        return (data != 0);
    }

    void dispose()
    {
        agg::int8u* data = buf();
        delete[] data;
    }

    unsigned m_pixel_size;
    XImage* m_img;
};
