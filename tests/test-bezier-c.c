#include "libelplot.h"

int main() {
    elp_initialize_fonts();

    elp_plot *plot = elp_plot_new(elp_plot_show_units);
    elp_plot_set_limits(plot, 0, 0, 100, 100);

    elp_curve *heart = elp_curve_new();
    elp_curve_move_to(heart, 10, 70);
    elp_curve_arc_to(heart, 20, 20, 0, false, false, 50, 70);
    elp_curve_arc_to(heart, 20, 20, 0, false, false, 90, 70);
    elp_curve_curve3(heart, 90, 40, 50, 10);
    elp_curve_curve3(heart, 10, 40, 10, 70);
    elp_curve_close_polygon(heart);

    elp_plot_add(plot, curve_as_object(heart), canvas_color_red, 2.5, 0xE40000FF, elp_stroke | elp_fill);

    elp_window *window = elp_window_new();
    elp_window_attach(window, plot, "");
    elp_window_start(window, 480, 480, elp_window_resize);
    elp_window_wait(window);

    elp_window_free(window);
    elp_plot_free(plot);
    return 0;
}
