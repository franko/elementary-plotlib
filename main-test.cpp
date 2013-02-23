#include "window_surface.h"
#include "graph_mutex.h"
#include "plot.h"
#include "path.h"

class dummy_window : public display_window {
public:
    dummy_window(): m_surf(0) {}

    void attach(window_surface* surf) { m_surf = surf; }

    virtual void update_region(const agg::rect_i& r)
    {
        printf("updating %d, %d, %d, %d.\n", r.x1, r.y1, r.x2, r.y2);
    }

private:
    window_surface* m_surf;
};

int main()
{
    graphics::initialize_fonts();

    dummy_window win;
    graph_mutex_simple mutex;

    window_surface surf(&win, mutex, "h..");
    win.attach(&surf);

    plot p(true);
    agg::rect_d lim(0.0, -1.0, 10.0, 1.0);
    p.set_limits(lim);

    draw::path* ln = new draw::path();
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
