#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "elem/elem.h"

struct bm_generator {
    bool have_z;
    double z1;
};

static double xfsin(double x) {
    return sin(x) / x;
}

static double box_muller(struct bm_generator *gen, double mu, double sigma) {
    const double tau = 2 * 3.14159265358979323846;
    if (gen->have_z) {
        gen->have_z = false;
        return gen->z1 * sigma + mu;
    }
    double u1 = rand() * (1.0 / RAND_MAX);
    double u2 = rand() * (1.0 / RAND_MAX);

    double z0 = sqrt(-2.0 * log(u1)) * cos(tau * u2);
    gen->z1 = sqrt(-2.0 * log(u1)) * sin(tau * u2);
    gen->have_z = true;
    return z0 * sigma + mu;
}

int main() {
    elem_initialize_fonts();

    elem_plot *plot = elem_plot_new(elem_plot_show_units | elem_plot_auto_limits);
    elem_plot_set_clip_mode(plot, false);

    srand(193);
    struct bm_generator gen[1] = {{false, 0.0}};

    elem_markers *markers = elem_markers_new(6, elem_marker_symbol_by_index(1));
    const double x0 = 0.0001, x1 = 8 * 2 * 3.14153265359;
    const int n_markers = 23;
    for (int i = 0; i < n_markers; i++) {
        const double x = x0 + i * (x1 - x0) / (n_markers - 1);
        elem_path_line_to(markers_as_path(markers), x, xfsin(x) + box_muller(gen, 0.0, 0.02));
    }    
    elem_plot_add(plot, markers_as_object(markers), elem_color_blue, 1.5, elem_color_none, elem_stroke);

    elem_path *line = elem_path_new();
    const int n_line = 256;
    for (int i = 0; i < n_line; i++) {
        const double x = x0 + i * (x1 - x0) / (n_line - 1);
        elem_path_line_to(line, x, xfsin(x));
    }
    elem_plot_add(plot, path_as_object(line), elem_color_red, 1.5, elem_color_none, elem_stroke);

    elem_plot_set_title(plot, "Function plot example");
    elem_plot_set_x_axis_title(plot, "x variable");

    elem_window *window = elem_window_new();
    elem_window_attach(window, plot, "");
    elem_window_start(window, 640, 480, elem_window_resize);
    elem_window_wait(window);

    elem_window_free(window);
    elem_plot_free(plot);
    return 0;
}
