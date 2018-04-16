#include "window_surface.h"
#include "graph_mutex.h"
#include "plot.h"
#include "sg_path.h"

class dummy_window : public graphics::display_window {
public:
    dummy_window() {}

    virtual void update_region(graphics::image& img, const agg::rect_i& r)
    {
        printf("updating %d, %d, %d, %d.\n", r.x1, r.y1, r.x2, r.y2);
    }
};

int main()
{
    graphics::initialize_fonts();

    dummy_window win;
    graphics::global_mutex mutex;

    graphics::window_surface surf(&win, mutex, "h..");

    graphics::plot p(true);
    agg::rect_d lim(0.0, -1.0, 10.0, 1.0);
    p.set_limits(lim);

    sg_path* ln = new sg_path();
    agg::path_storage& l = ln->self();
    l.move_to(-0.5, 0.0);
    l.line_to(-0.5, 8.0);
    l.line_to(0.5, 4.0);
    l.close_polygon();

    agg::rgba8 red(180, 0, 0, 255);
    p.add(ln, red, true);

    surf.attach(&p, "1");

    surf.resize(600, 500);
    surf.draw_all();

    return 0;
}
