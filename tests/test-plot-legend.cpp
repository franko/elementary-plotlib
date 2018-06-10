#include <cmath>

#include "window.h"
#include "debug_log.h"
#include "path.h"
#include "text.h"

template <typename Function>
void add_function(graphics::plot& p, double x0, double x1, Function f, agg::rgba8 color, int n = 512) {
    agg::rgba8 none(0,0,0,0);
    auto line = new graphics::path();
    line->move_to(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line->line_to(x, f(x));
    }
    p.add(line, color, 1.5, none, graphics::property::stroke);
}

graphics::plot *create_legend() {
    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 none(0,0,0,0);
    agg::rgba8 black(0,0,0,255);
    auto p = new graphics::plot {0};
    p->set_limits({0.0, -5.0, 16.0, 5.0});
    p->set_clip_mode(false);
    p->add(new graphics::path {{0.0, -3.0}, {3.0, -3.0}}, blue, 1.5, none, graphics::property::stroke);
    p->add(new graphics::path {{0.0, +3.0}, {3.0, +3.0}}, red,  1.5, none, graphics::property::stroke);
    auto text1 = new draw::text {"sin(x) / x", 14.0, 0.0, 0.3};
    text1->set_point(4.0, -3.0);
    p->add(text1, black, 1.0, black, graphics::property::fill);
    auto text2 = new draw::text {"cos(x) / x", 14.0, 0.0, 0.3};
    text2->set_point(4.0, +3.0);
    p->add(text2, black, 1.0, black, graphics::property::fill);
    p->commit_pending_draw();
    return p;
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

    p.set_title("Function plot example");
    p.set_x_axis_title("x variable");

    p.add_legend(create_legend(), graphics::plot::right);

    p.commit_pending_draw();

    int index = win.attach(&p, "");

    win.start(640, 480, graphics::window_resize);

    add_function(p, 0.8, x1, [](double x) { return std::cos(x) / x; }, red);
    win.slot_refresh(index);
    p.commit_pending_draw();

    win.wait();

    return 0;
}
