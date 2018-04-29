#include "window.h"
#include "debug_log.h"
#include "path.h"

int main()
{
    graphics::initialize_fonts();

    graphics::window win("h.");

    graphics::plot p(graphics::plot::show_units | graphics::plot::auto_limits);
    p.set_clip_mode(false);

    agg::rgba8 none(0,0,0,0);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 yellow(245, 254, 0, 255);

    auto line = new graphics::path();
    line->move_to(-0.5, 0.0);
    line->line_to(-0.5, 8.0);
    line->line_to(0.5, 4.0);
    line->close_polygon();
    p.add(line, red, 2.5, yellow, graphics::property::fill | graphics::property::stroke);

    p.commit_pending_draw();

    int index = win.attach(&p, "1");

    win.start(640, 480, graphics::window_resize);

    sleep(4);

    auto line2 = new graphics::path();
    line2->move_to(0.8, 1.0);
    line2->line_to(0.8, 7.0);
    line2->line_to(0.3, 4.0);
    line2->close_polygon();
    p.add(line2, blue, 2.5, none);

    win.slot_refresh(index);
    p.commit_pending_draw();

    win.wait();

    return 0;
}
