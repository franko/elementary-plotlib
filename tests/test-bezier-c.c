#include "libcanvas.h"

int main() {
    canvas_initialize_fonts();

    canvas_plot *plot = canvas_plot_new(canvas_plot_show_units);
    canvas_plot_set_limits(plot, 0, 0, 100, 100);

    canvas_curve *heart = canvas_curve_new();
    canvas_curve_move_to(heart, 10, 70);
    canvas_curve_arc_to(heart, 20, 20, 0, false, false, 50, 70);
    canvas_curve_arc_to(heart, 20, 20, 0, false, false, 90, 70);
    canvas_curve_curve3(heart, 90, 40, 50, 10);
    canvas_curve_curve3(heart, 10, 40, 10, 70);
    canvas_curve_close_polygon(heart);

    canvas_plot_add(plot, curve_as_object(heart), canvas_color_red, 2.5, 0xE40000FF, canvas_stroke | canvas_fill);

    canvas_window *window = canvas_window_new();
    canvas_window_attach(window, plot, "");
    canvas_window_start(window, 480, 480, canvas_window_resize);
    canvas_window_wait(window);

    canvas_window_free(window);
    canvas_plot_free(plot);
    return 0;
}
