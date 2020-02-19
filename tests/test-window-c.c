// #include "elem_plot_utils.h"
#include "elem_plot.h"

int main() {
    elem_initialize_fonts();

    elem_plot *plot = elem_plot_new(elem_plot_show_units);
    elem_plot_set_limits(plot, -1, 0, 1, 10);

    elem_plot_set_label_angle(plot, elem_x_axis, 3.141592 / 4);
    elem_plot_enable_label_format(plot, elem_x_axis, "%.2f");

    elem_path *line = elem_path_new();
    elem_path_move_to(line, -0.5, 0.0);
    elem_path_line_to(line, -0.5, 8.0);
    elem_path_line_to(line,  0.5, 4.0);
    elem_path_close_polygon(line);
    elem_plot_add(plot, path_as_object(line), elem_color_red, 2.5, elem_color_yellow, elem_fill | elem_stroke);

    elem_window *window = elem_window_new();
    elem_window_attach(window, plot, "");
    elem_window_start(window, 640, 480, elem_window_resize);

    elem_path *line2 = elem_path_new();
    elem_path_move_to(line2, 0.8, 1.0);
    elem_path_line_to(line2, 0.8, 7.0);
    elem_path_line_to(line2, 0.3, 4.0);
    elem_path_close_polygon(line2);
    elem_plot_add(plot, path_as_object(line2), elem_color_blue, 2.5, elem_color_none, elem_stroke);

    elem_window_wait(window);

    elem_window_free(window);
    elem_plot_free(plot);
    return 0;
}
