// #include "libelplot_utils.h"
#include "libelplot.h"

int main() {
    elp_initialize_fonts();

    elp_plot *plot = elp_plot_new(elp_plot_show_units);
    elp_plot_set_limits(plot, -1, 0, 1, 10);

    elp_plot_set_label_angle(plot, canvas_x_axis, 3.141592 / 4);
    elp_plot_enable_label_format(plot, canvas_x_axis, "%.2f");

    elp_path *line = elp_path_new();
    elp_path_move_to(line, -0.5, 0.0);
    elp_path_line_to(line, -0.5, 8.0);
    elp_path_line_to(line,  0.5, 4.0);
    elp_path_close_polygon(line);
    elp_plot_add(plot, path_as_object(line), canvas_color_red, 2.5, canvas_color_yellow, elp_fill | elp_stroke);

    elp_window *window = elp_window_new();
    elp_window_attach(window, plot, "");
    elp_window_start(window, 640, 480, elp_window_resize);

    elp_path *line2 = elp_path_new();
    elp_path_move_to(line2, 0.8, 1.0);
    elp_path_line_to(line2, 0.8, 7.0);
    elp_path_line_to(line2, 0.3, 4.0);
    elp_path_close_polygon(line2);
    elp_plot_add(plot, path_as_object(line2), canvas_color_blue, 2.5, canvas_color_none, elp_stroke);

    elp_window_wait(window);

    elp_window_free(window);
    elp_plot_free(plot);
    return 0;
}
