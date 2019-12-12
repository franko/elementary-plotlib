#include <math.h>

#include "libelplot.h"

static double xfsin(double x) {
    return sin(x) / x;
}

static double xfcos(double x) {
    return cos(x) / x;
}

int main() {
    elp_initialize_fonts();

    elp_plot *plot = elp_plot_new(elp_plot_show_units | elp_plot_auto_limits);
    elp_plot_set_clip_mode(plot, false);

    // TODO add PI definition
    const double x0 = 0.8, x1 = 8 * 2 * 3.14159265359;
    elp_dashed *spath = elp_dashed_new(8, 4);
    elp_path_move_to(dashed_as_path(spath), x0, xfsin(x0));
    const int n = 256;
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        elp_path_line_to(dashed_as_path(spath), x, xfsin(x));
    }
    elp_plot_add(plot, dashed_as_object(spath), elp_color_blue, 1.5, elp_color_none, elp_stroke);

    elp_plot_set_title(plot, "Function plot example");
    elp_plot_set_x_axis_title(plot, "x variable");
    elp_plot_set_y_axis_title(plot, "y variable");

    elp_window *window = elp_window_new();
    elp_window_attach(window, plot, "");
    elp_window_start(window, 640, 480, elp_window_resize);

    elp_dashed *cpath = elp_dashed_new(8, 4);
    elp_dashed_add_dash(cpath, 2, 4);
    elp_path_move_to(dashed_as_path(cpath), x0, xfcos(x0));
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        elp_path_line_to(dashed_as_path(cpath), x, xfcos(x));
    }
    elp_plot_add(plot, dashed_as_object(cpath), elp_color_red, 1.5, elp_color_none, elp_stroke);
    elp_window_wait(window);

    elp_window_free(window);
    elp_plot_free(plot);
    return 0;
}
