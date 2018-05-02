#include <cmath>
#include <random>

#include "window.h"
#include "debug_log.h"
#include "path.h"
#include "markers.h"

template <typename Path, typename Function>
void path_function(Path *line, Function f, double x0, double x1, int n = 128) {
    line->move_to(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line->line_to(x, f(x));
    }
}

int main()
{
    graphics::initialize_fonts();

    graphics::window win;
    graphics::plot p(graphics::plot::show_units | graphics::plot::auto_limits);
    p.set_clip_mode(false);

    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 none(0,0,0,0);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    auto line = new graphics::path{};
    path_function(line, [](double x) { return std::sin(x) / x; }, x0, x1, 256);

    std::random_device r;
    std::default_random_engine e1(r());
    std::normal_distribution<double> normal_dist(0.0, 0.02);

    auto marker_symbol = new_marker_symbol(1);
    auto markers = new graphics::markers{6.0, marker_symbol};
    path_function(markers, [&](double x) { return std::sin(x) / x + normal_dist(e1); }, x0, x1, 23);

    p.add(line, red, 1.5, none, graphics::property::stroke);
    p.add(markers, blue, 1.5, none, graphics::property::stroke);

    p.set_title("Function plot example");
    p.set_x_axis_title("x variable");

    p.commit_pending_draw();

    win.attach(&p, "");
    win.start(640, 480, graphics::window_resize);
    win.wait();

    return 0;
}
