#include "util/agg_color_conv_rgb8.h"

#include "rendering_buffer_utils.h"

void rendering_buffer_copy(agg::rendering_buffer& dst, agg::pix_format_e dst_format, const agg::rendering_buffer& src, agg::pix_format_e src_format)
{
    if (dst_format == src_format)
    {
        dst.copy_from(src);
    }
    else
    {
        switch(dst_format)
        {
        default:
            break;
        case agg::pix_format_rgb555:
            switch(src_format)
            {
            default:
                break;
            case agg::pix_format_rgb555:
                my_color_conv(&dst, &src, agg::color_conv_rgb555_to_rgb555());
                break;
            case agg::pix_format_rgb565:
                my_color_conv(&dst, &src, agg::color_conv_rgb565_to_rgb555());
                break;
            case agg::pix_format_rgb24:
                my_color_conv(&dst, &src, agg::color_conv_rgb24_to_rgb555());
                break;
            case agg::pix_format_bgr24:
                my_color_conv(&dst, &src, agg::color_conv_bgr24_to_rgb555());
                break;
            case agg::pix_format_rgba32:
                my_color_conv(&dst, &src, agg::color_conv_rgba32_to_rgb555());
                break;
            case agg::pix_format_argb32:
                my_color_conv(&dst, &src, agg::color_conv_argb32_to_rgb555());
                break;
            case agg::pix_format_bgra32:
                my_color_conv(&dst, &src, agg::color_conv_bgra32_to_rgb555());
                break;
            case agg::pix_format_abgr32:
                my_color_conv(&dst, &src, agg::color_conv_abgr32_to_rgb555());
                break;
            }
            break;

        case agg::pix_format_rgb565:
            switch(src_format)
            {
            default:
                break;
            case agg::pix_format_rgb555:
                my_color_conv(&dst, &src, agg::color_conv_rgb555_to_rgb565());
                break;
            case agg::pix_format_rgb565:
                my_color_conv(&dst, &src, agg::color_conv_rgb565_to_rgb565());
                break;
            case agg::pix_format_rgb24:
                my_color_conv(&dst, &src, agg::color_conv_rgb24_to_rgb565());
                break;
            case agg::pix_format_bgr24:
                my_color_conv(&dst, &src, agg::color_conv_bgr24_to_rgb565());
                break;
            case agg::pix_format_rgba32:
                my_color_conv(&dst, &src, agg::color_conv_rgba32_to_rgb565());
                break;
            case agg::pix_format_argb32:
                my_color_conv(&dst, &src, agg::color_conv_argb32_to_rgb565());
                break;
            case agg::pix_format_bgra32:
                my_color_conv(&dst, &src, agg::color_conv_bgra32_to_rgb565());
                break;
            case agg::pix_format_abgr32:
                my_color_conv(&dst, &src, agg::color_conv_abgr32_to_rgb565());
                break;
            }
            break;

        case agg::pix_format_rgba32:
            switch(src_format)
            {
            default:
                break;
            case agg::pix_format_rgb555:
                my_color_conv(&dst, &src, agg::color_conv_rgb555_to_rgba32());
                break;
            case agg::pix_format_rgb565:
                my_color_conv(&dst, &src, agg::color_conv_rgb565_to_rgba32());
                break;
            case agg::pix_format_rgb24:
                my_color_conv(&dst, &src, agg::color_conv_rgb24_to_rgba32());
                break;
            case agg::pix_format_bgr24:
                my_color_conv(&dst, &src, agg::color_conv_bgr24_to_rgba32());
                break;
            case agg::pix_format_rgba32:
                my_color_conv(&dst, &src, agg::color_conv_rgba32_to_rgba32());
                break;
            case agg::pix_format_argb32:
                my_color_conv(&dst, &src, agg::color_conv_argb32_to_rgba32());
                break;
            case agg::pix_format_bgra32:
                my_color_conv(&dst, &src, agg::color_conv_bgra32_to_rgba32());
                break;
            case agg::pix_format_abgr32:
                my_color_conv(&dst, &src, agg::color_conv_abgr32_to_rgba32());
                break;
            }
            break;

        case agg::pix_format_abgr32:
            switch(src_format)
            {
            default:
                break;
            case agg::pix_format_rgb555:
                my_color_conv(&dst, &src, agg::color_conv_rgb555_to_abgr32());
                break;
            case agg::pix_format_rgb565:
                my_color_conv(&dst, &src, agg::color_conv_rgb565_to_abgr32());
                break;
            case agg::pix_format_rgb24:
                my_color_conv(&dst, &src, agg::color_conv_rgb24_to_abgr32());
                break;
            case agg::pix_format_bgr24:
                my_color_conv(&dst, &src, agg::color_conv_bgr24_to_abgr32());
                break;
            case agg::pix_format_abgr32:
                my_color_conv(&dst, &src, agg::color_conv_abgr32_to_abgr32());
                break;
            case agg::pix_format_rgba32:
                my_color_conv(&dst, &src, agg::color_conv_rgba32_to_abgr32());
                break;
            case agg::pix_format_argb32:
                my_color_conv(&dst, &src, agg::color_conv_argb32_to_abgr32());
                break;
            case agg::pix_format_bgra32:
                my_color_conv(&dst, &src, agg::color_conv_bgra32_to_abgr32());
                break;
            }
            break;

        case agg::pix_format_argb32:
            switch(src_format)
            {
            default:
                break;
            case agg::pix_format_rgb555:
                my_color_conv(&dst, &src, agg::color_conv_rgb555_to_argb32());
                break;
            case agg::pix_format_rgb565:
                my_color_conv(&dst, &src, agg::color_conv_rgb565_to_argb32());
                break;
            case agg::pix_format_rgb24:
                my_color_conv(&dst, &src, agg::color_conv_rgb24_to_argb32());
                break;
            case agg::pix_format_bgr24:
                my_color_conv(&dst, &src, agg::color_conv_bgr24_to_argb32());
                break;
            case agg::pix_format_rgba32:
                my_color_conv(&dst, &src, agg::color_conv_rgba32_to_argb32());
                break;
            case agg::pix_format_argb32:
                my_color_conv(&dst, &src, agg::color_conv_argb32_to_argb32());
                break;
            case agg::pix_format_abgr32:
                my_color_conv(&dst, &src, agg::color_conv_abgr32_to_argb32());
                break;
            case agg::pix_format_bgra32:
                my_color_conv(&dst, &src, agg::color_conv_bgra32_to_argb32());
                break;
            }
            break;

        case agg::pix_format_bgra32:
            switch(src_format)
            {
            default:
                break;
            case agg::pix_format_rgb555:
                my_color_conv(&dst, &src, agg::color_conv_rgb555_to_bgra32());
                break;
            case agg::pix_format_rgb565:
                my_color_conv(&dst, &src, agg::color_conv_rgb565_to_bgra32());
                break;
            case agg::pix_format_rgb24:
                my_color_conv(&dst, &src, agg::color_conv_rgb24_to_bgra32());
                break;
            case agg::pix_format_bgr24:
                my_color_conv(&dst, &src, agg::color_conv_bgr24_to_bgra32());
                break;
            case agg::pix_format_rgba32:
                my_color_conv(&dst, &src, agg::color_conv_rgba32_to_bgra32());
                break;
            case agg::pix_format_argb32:
                my_color_conv(&dst, &src, agg::color_conv_argb32_to_bgra32());
                break;
            case agg::pix_format_abgr32:
                my_color_conv(&dst, &src, agg::color_conv_abgr32_to_bgra32());
                break;
            case agg::pix_format_bgra32:
                my_color_conv(&dst, &src, agg::color_conv_bgra32_to_bgra32());
                break;
            }
            break;
        }
    }

}
