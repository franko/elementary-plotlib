// DEBUG: to be removed / used just to have sleep()
#include <unistd.h>

#include "xwindow.h"
#include "window_surface.h"
#include "graph_mutex.h"
#include "plot.h"
#include "path.h"
#include "pthreadpp.h"

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

    virtual void render()
    {
        m_surface.render_all();
    }

    virtual void draw()
    {
        m_surface.draw_request();
    }

private:
    graphics::window_surface& m_surface;
};

struct xwindow_thread : public pthread::thread {
    xwindow_thread(xwindow& win): m_window(win) {}
    virtual void run() {
        m_window.init(640, 480, xwindow::window_resize);
        m_window.run();
        m_window.close();
    }
private:
    xwindow& m_window;
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
    agg::rect_d lim(-1.0, 0.0, 1.0, 10.0);
    p.set_limits(lim);

    draw::path* ln = new draw::path();
    agg::path_storage& l = ln->self();
    l.move_to(-0.5, 0.0);
    l.line_to(-0.5, 8.0);
    l.line_to(0.5, 4.0);
    l.close_polygon();

    trans::scaling* ln_s = new trans::scaling(ln);

    agg::rgba8 red(180, 0, 0, 255);
    p.add(ln_s, red, true);

    p.commit_pending_draw();

    int index = surf.attach(&p, "1");

    xwindow_thread thread(xwin);

    sleep(4);

    draw::path* ln2 = new draw::path();
    agg::path_storage& l2 = ln2->self();
    l2.move_to(0.8, 1.0);
    l2.line_to(0.8, 7.0);
    l2.line_to(0.3, 4.0);
    l2.close_polygon();

    trans::scaling* ln2_s = new trans::scaling(ln2);

    agg::rgba8 blue(0, 0, 180, 255);
    p.add(ln2_s, blue, true);

    surf.slot_refresh(index);
    p.commit_pending_draw();

    for (;;) {
        fprintf(stderr, "sleeping...\n");
        sleep(2);
    }

    return 0;
}
