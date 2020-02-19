#include <math.h>

#include "elem_plot.h"

static double xfsin(double x) {
    return sin(x) / x;
}

static double xfcos(double x) {
    return cos(x) / x;
}

int main() {
    elem_initialize_fonts();

    elem_plot *plot = elem_plot_new(elem_plot_show_units | elem_plot_auto_limits);
    elem_plot_set_clip_mode(plot, false);

    // TODO add PI definition
    const double x0 = 0.8, x1 = 8 * 2 * 3.14159265359;
    elem_dashed *spath = elem_dashed_new(8, 4);
    elem_path_move_to(dashed_as_path(spath), x0, xfsin(x0));
    const int n = 256;
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        elem_path_line_to(dashed_as_path(spath), x, xfsin(x));
    }
    elem_plot_add(plot, dashed_as_object(spath), elem_color_blue, 1.5, elem_color_none, elem_stroke);

    elem_plot_set_title(plot, "Function plot example");
    elem_plot_set_x_axis_title(plot, "x variable");
    elem_plot_set_y_axis_title(plot, "y variable");

    elem_window *window = elem_window_new();
    elem_window_attach(window, plot, "");
    elem_window_start(window, 640, 480, elem_window_resize);

    elem_dashed *cpath = elem_dashed_new(8, 4);
    elem_dashed_add_dash(cpath, 2, 4);
    elem_path_move_to(dashed_as_path(cpath), x0, xfcos(x0));
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        elem_path_line_to(dashed_as_path(cpath), x, xfcos(x));
    }
    elem_plot_add(plot, dashed_as_object(cpath), elem_color_red, 1.5, elem_color_none, elem_stroke);
    elem_window_wait(window);

    elem_window_free(window);
    elem_plot_free(plot);
    return 0;
}
