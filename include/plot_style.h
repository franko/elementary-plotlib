#pragma once

enum text_class_e { text_axis_title, text_axis_labels, text_plot_title };

inline double get_default_font_size(text_class_e tc, double scale)
{
    const int base_size = \
        (tc == text_axis_title ? 15 : \
        (tc == text_axis_labels ? 14 : 18));
    const double cscale = max(scale, 0.75);
    return base_size * cscale;
}

inline double std_line_width(double scale, double w = 1.0)
{
    return w * 1.5;
}
