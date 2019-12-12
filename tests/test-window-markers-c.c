#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "libelplot.h"

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
    elp_initialize_fonts();

    elp_plot *plot = elp_plot_new(elp_plot_show_units | elp_plot_auto_limits);
    elp_plot_set_clip_mode(plot, false);

    srand(193);
    struct bm_generator gen[1] = {{false, 0.0}};

    elp_markers *markers = elp_markers_new(6, elp_marker_symbol_by_index(1));
    const double x0 = 0.0001, x1 = 8 * 2 * 3.14153265359;
    const int n_markers = 23;
    for (int i = 0; i < n_markers; i++) {
        const double x = x0 + i * (x1 - x0) / (n_markers - 1);
        elp_path_line_to(markers_as_path(markers), x, xfsin(x) + box_muller(gen, 0.0, 0.02));
    }    
    elp_plot_add(plot, markers_as_object(markers), elp_color_blue, 1.5, elp_color_none, elp_stroke);

    elp_path *line = elp_path_new();
    const int n_line = 256;
    for (int i = 0; i < n_line; i++) {
        const double x = x0 + i * (x1 - x0) / (n_line - 1);
        elp_path_line_to(line, x, xfsin(x));
    }
    elp_plot_add(plot, path_as_object(line), elp_color_red, 1.5, elp_color_none, elp_stroke);

    elp_plot_set_title(plot, "Function plot example");
    elp_plot_set_x_axis_title(plot, "x variable");

    elp_window *window = elp_window_new();
    elp_window_attach(window, plot, "");
    elp_window_start(window, 640, 480, elp_window_resize);
    elp_window_wait(window);

    elp_window_free(window);
    elp_plot_free(plot);
    return 0;
}
