#include "elem_plot.h"

int main() {
    elem_initialize_fonts();

    elem_plot *plot = elem_plot_new(elem_plot_show_units);
    elem_plot_set_limits(plot, 0, 0, 100, 100);

    elem_curve *heart = elem_curve_new();
    elem_curve_move_to(heart, 10, 70);
    elem_curve_arc_to(heart, 20, 20, 0, false, false, 50, 70);
    elem_curve_arc_to(heart, 20, 20, 0, false, false, 90, 70);
    elem_curve_curve3(heart, 90, 40, 50, 10);
    elem_curve_curve3(heart, 10, 40, 10, 70);
    elem_curve_close_polygon(heart);

    elem_plot_add(plot, curve_as_object(heart), elem_color_red, 2.5, 0xE40000FF, elem_stroke | elem_fill);

    elem_window *window = elem_window_new();
    elem_window_attach(window, plot, "");
    elem_window_start(window, 480, 480, elem_window_resize);
    elem_window_wait(window);

    elem_window_free(window);
    elem_plot_free(plot);
    return 0;
}
