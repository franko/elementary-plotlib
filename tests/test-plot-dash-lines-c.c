#include <math.h>

#include "libcanvas.h"

static double xfsin(double x) {
    return sin(x) / x;
}

static double xfcos(double x) {
    return cos(x) / x;
}

int main() {
    canvas_initialize_fonts();

    canvas_plot *plot = canvas_plot_new(canvas_plot_show_units | canvas_plot_auto_limits);
    canvas_plot_set_clip_mode(plot, false);

    // TODO add PI definition
    const double x0 = 0.8, x1 = 8 * 2 * 3.14159265359;
    canvas_dashed *spath = canvas_dashed_new(8, 4);
    canvas_path_move_to(dashed_as_path(spath), x0, xfsin(x0));
    const int n = 256;
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        canvas_path_line_to(dashed_as_path(spath), x, xfsin(x));
    }
    canvas_plot_add(plot, dashed_as_object(spath), canvas_color_blue, 1.5, canvas_color_none, canvas_stroke);

    canvas_plot_set_title(plot, "Function plot example");
    canvas_plot_set_x_axis_title(plot, "x variable");
    canvas_plot_set_y_axis_title(plot, "y variable");

    canvas_window *window = canvas_window_new();
    canvas_window_attach(window, plot, "");
    canvas_window_start(window, 640, 480, canvas_window_resize);

    canvas_dashed *cpath = canvas_dashed_new(8, 4);
    canvas_dashed_add_dash(cpath, 2, 4);
    canvas_path_move_to(dashed_as_path(cpath), x0, xfcos(x0));
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        canvas_path_line_to(dashed_as_path(cpath), x, xfcos(x));
    }
    canvas_plot_add(plot, dashed_as_object(cpath), canvas_color_red, 1.5, canvas_color_none, canvas_stroke);
    canvas_window_wait(window);

    canvas_window_free(window);
    canvas_plot_free(plot);
    return 0;
}
