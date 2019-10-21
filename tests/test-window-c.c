// #include "libcanvas_utils.h"
#include "libcanvas.h"

int main() {
    canvas_initialize_fonts();

    canvas_plot *plot = canvas_plot_new(canvas_plot_show_units);
    canvas_plot_set_limits(plot, -1, 0, 1, 10);

    canvas_plot_set_label_angle(plot, canvas_x_axis, 3.141592 / 4);
    canvas_plot_enable_label_format(plot, canvas_x_axis, "%.2f");

    canvas_path *line = canvas_path_new();
    canvas_path_move_to(line, -0.5, 0.0);
    canvas_path_line_to(line, -0.5, 8.0);
    canvas_path_line_to(line,  0.5, 4.0);
    canvas_path_close_polygon(line);
    canvas_plot_add(plot, path_as_object(line), canvas_color_red, 2.5, canvas_color_yellow, canvas_fill | canvas_stroke);

    canvas_window *window = canvas_window_new();
    canvas_window_attach(window, plot, "");
    canvas_window_start(window, 640, 480, canvas_window_resize);

    canvas_path *line2 = canvas_path_new();
    canvas_path_move_to(line2, 0.8, 1.0);
    canvas_path_line_to(line2, 0.8, 7.0);
    canvas_path_line_to(line2, 0.3, 4.0);
    canvas_path_close_polygon(line2);
    canvas_plot_add(plot, path_as_object(line2), canvas_color_blue, 2.5, canvas_color_none, canvas_stroke);

    canvas_window_wait(window);

    canvas_window_free(window);
    canvas_plot_free(plot);
    return 0;
}
