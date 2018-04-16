#include <cmath>

#include "window.h"
#include "debug_log.h"
#include "path.h"

static double f(double x) { return std::sin(x) / x; }

int main()
{
    graphics::initialize_fonts();

    graphics::window win("h.");

    graphics::plot p(graphics::plot::show_units | graphics::plot::auto_limits);
    p.set_clip_mode(false);

    agg::rgba8 none(0,0,0,0);
    agg::rgba8 blue(0, 0, 180, 255);

    graphics::path line(none, blue, 1.5, ELEMENT_STROKE);
    const int n = 512;
    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    line.move_to(x0, f(x0));
    for (int i = 0; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line.line_to(x, f(x));
    }
    p.add(line);

    p.commit_pending_draw();

    int index = win.attach(&p, "1");

    win.start(640, 480, graphics::window_resize);

    sleep(4);
    win.slot_refresh(index);

    for (;;) {
        debug_log("sleeping...");
        sleep(2);
    }

    return 0;
}
