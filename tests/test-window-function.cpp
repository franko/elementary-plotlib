#include <cmath>

#include "window.h"
#include "debug_log.h"
#include "path.h"

template <typename Function>
void add_function(graphics::plot& p, double x0, double x1, Function f, agg::rgba8 color, int n = 512) {
    agg::rgba8 none(0,0,0,0);
    auto line = new graphics::path(none, color, 1.5, graphics::property::stroke);
    line->move_to(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line->line_to(x, f(x));
    }
    p.add(line);
}

int main()
{
    graphics::initialize_fonts();

    graphics::window win;
    graphics::plot p(graphics::plot::show_units | graphics::plot::auto_limits);
    p.set_clip_mode(false);

    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 red(180, 0, 0, 255);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    add_function(p, x0, x1, [](double x) { return std::sin(x) / x; }, blue);
    add_function(p, 0.8, x1, [](double x) { return std::cos(x) / x; }, red);

    p.set_title("Function plot example");
    p.set_x_axis_title("x variable");

    p.commit_pending_draw();

    int index = win.attach(&p, "");

    win.start(640, 480, graphics::window_resize);

    // Not needed.
    win.slot_refresh(index);

    win.wait();

    return 0;
}
