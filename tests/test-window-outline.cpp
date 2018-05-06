#include <cmath>
#include <memory>

#include "window.h"
#include "debug_log.h"
#include "path.h"

template <typename Function>
void add_function(graphics::plot& p, double x0, double x1, Function f, agg::rgba8 color, unsigned flags, int n = 512) {
    agg::rgba8 none(0,0,0,0);
    auto line = new graphics::path();
    line->move_to(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line->line_to(x, f(x));
    }
    p.add(line, color, 1.5, none, flags);
}

std::unique_ptr<graphics::plot> create_function_plot(const char *title, unsigned flags) {
    auto p = std::make_unique<graphics::plot>(graphics::plot::show_units | graphics::plot::auto_limits);
    p->set_clip_mode(false);
    p->set_title(title);

    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 red(180, 0, 0, 255);
    const double x0 = 3.14159265358979323846/2, x1 = 8 * 2 * 3.14159265358979323846;
    auto fsin = [](double x) { return std::sin(x) / x; };
    auto fcos = [](double x) { return std::cos(x) / x; };
    add_function(*p, x0, x1, fsin, blue, flags);
    add_function(*p, x0, x1, fcos, red,  flags);

    p->commit_pending_draw();
    return p;
}

int main()
{
    graphics::initialize_fonts();

    graphics::window win("v...");

    auto plot1 = create_function_plot("Plot example (antialias)", graphics::property::stroke);
    auto plot2 = create_function_plot("Plot example (outline)", graphics::property::outline);
    auto plot3 = create_function_plot("Plot example (aliased)", graphics::property::stroke|graphics::property::crisp);

    win.attach(plot1.get(), "3");
    win.attach(plot2.get(), "2");
    win.attach(plot3.get(), "1");

    win.start(480, 3 * 320, graphics::window_resize);
    win.wait();

    return 0;
}
