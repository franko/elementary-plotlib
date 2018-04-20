#include "window.h"
#include "debug_log.h"
#include "path.h"

int main()
{
    graphics::initialize_fonts();

    graphics::window win;

    graphics::plot p(graphics::plot::show_units);
    agg::rect_d lim(-1.0, 0.0, 1.0, 10.0);
    p.set_limits(lim);
    p.set_axis_labels_angle(graphics::x_axis, 3.141592 / 4);
    p.enable_label_format(graphics::x_axis, "%.6f");

    agg::rgba8 none(0,0,0,0);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 yellow(245, 254, 0, 255);

    auto line = new graphics::path(yellow, red, 2.5, ELEMENT_FILL|ELEMENT_STROKE);
    line->move_to(-0.5, 0.0);
    line->line_to(-0.5, 8.0);
    line->line_to(0.5, 4.0);
    line->close_polygon();
    p.add(line);

    p.commit_pending_draw();

    int index = win.attach(&p, "");

    win.start(640, 480, graphics::window_resize);

    sleep(4);

    auto line2 = new graphics::path(none, blue, 2.5);
    line2->move_to(0.8, 1.0);
    line2->line_to(0.8, 7.0);
    line2->line_to(0.3, 4.0);
    line2->close_polygon();
    p.add(line2);

    win.slot_refresh(index);
    p.commit_pending_draw();

    for (;;) {
        debug_log("sleeping...");
        sleep(2);
    }

    return 0;
}
