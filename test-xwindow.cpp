#include "xwindow.h"
#include "window_surface.h"
#include "graph_mutex.h"
#include "plot.h"
#include "path.h"

class xwindow_display : public graphics::display_window {
public:
    xwindow_display(xwindow& xwin): m_xwindow(xwin) { }

    virtual void update_region(graphics::image& img, const agg::rect_i& r)
    {
        m_xwindow.update_region(img, r);
    }

    virtual void lock()   { m_xwindow.lock();   }
    virtual void unlock() { m_xwindow.unlock(); }

private:
    xwindow& m_xwindow;
};

class window_surface_target : public render_target {
public:
    window_surface_target(graphics::window_surface& surf):
    m_surface(surf)
    { }

    virtual void resize(unsigned width, unsigned height)
    {
        m_surface.resize(width, height);
    }

    virtual void draw()
    {
        m_surface.draw_all();
    }

private:
    graphics::window_surface& m_surface;
};

int main()
{
    graphics::initialize_fonts();

    // TODO: rename global mutex to something more meaningful
    graphics::global_mutex mutex;
    graphics::window_surface surf(mutex, "h..");
    window_surface_target surf_target(surf);

    xwindow xwin(surf_target);
    xwindow_display xwin_display(xwin);
    surf.attach_window(&xwin_display);

    graphics::plot p(true);
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

    xwin.init(640, 480, xwindow::window_resize);
    xwin.run();

    return 0;
}
