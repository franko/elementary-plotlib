#include <agg_array.h>

#include "window.h"
#include "debug_log.h"
#include "path.h"
#include "unique_ptr_container.h"

int main() {
    unique_ptr_container<agg::pod_bvector, graphics::plot> plot_list;

    graphics::initialize_fonts();

    auto p =  new graphics::plot(graphics::plot::show_units);
    plot_list.push_back(p);
    p->set_limits({-1.0, 0.0, 1.0, 10.0});

    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 yellow(245, 254, 0, 255);

    auto line = new graphics::polygon {{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    p->add(line, red, 2.5, yellow, graphics::property::fill | graphics::property::stroke);
    p->commit_pending_draw();

    graphics::window win;
    win.attach(p, "");
    win.start(640, 480, graphics::window_resize);
    win.wait();

    return 0;
}
