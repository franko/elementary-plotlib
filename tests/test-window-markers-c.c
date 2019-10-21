#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "libcanvas.h"

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
    canvas_initialize_fonts();

    canvas_plot *plot = canvas_plot_new(canvas_plot_show_units | canvas_plot_auto_limits);
    canvas_plot_set_clip_mode(plot, false);

    srand(193);
    struct bm_generator gen[1] = {{false, 0.0}};

    canvas_markers *markers = canvas_markers_new(6, canvas_marker_symbol_by_index(1));
    const double x0 = 0.0001, x1 = 8 * 2 * 3.14153265359;
    const int n_markers = 23;
    for (int i = 0; i < n_markers; i++) {
        const double x = x0 + i * (x1 - x0) / (n_markers - 1);
        canvas_path_line_to(markers_as_path(markers), x, xfsin(x) + box_muller(gen, 0.0, 0.02));
    }    
    canvas_plot_add(plot, markers_as_object(markers), canvas_color_blue, 1.5, canvas_color_none, canvas_stroke);

    canvas_path *line = canvas_path_new();
    const int n_line = 256;
    for (int i = 0; i < n_line; i++) {
        const double x = x0 + i * (x1 - x0) / (n_line - 1);
        canvas_path_line_to(line, x, xfsin(x));
    }
    canvas_plot_add(plot, path_as_object(line), canvas_color_red, 1.5, canvas_color_none, canvas_stroke);

    canvas_plot_set_title(plot, "Function plot example");
    canvas_plot_set_x_axis_title(plot, "x variable");

    canvas_window *window = canvas_window_new();
    canvas_window_attach(window, plot, "");
    canvas_window_start(window, 640, 480, canvas_window_resize);
    canvas_window_wait(window);

    canvas_window_free(window);
    canvas_plot_free(plot);
    return 0;
}
