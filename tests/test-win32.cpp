#include "win32/window_win32.h"
#include "window_surface.h"
#include "plot.h"
#include "path.h"
#include "pthreadpp.h"

class xwindow_display : public graphics::display_window {
public:
    xwindow_display(window_win32& xwin): m_xwindow(xwin) { }

    virtual void update_region(graphics::image& img, const agg::rect_i& r)
    {
        m_xwindow.update_region(img, r);
    }

    virtual void lock()   { m_xwindow.lock();   }
    virtual void unlock() { m_xwindow.unlock(); }

private:
    window_win32& m_xwindow;
};

class window_surface_target : public graphics::render_target {
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
    xwindow_thread(window_win32& win): m_window(win) {}
    virtual void run() {
        m_window.init(640, 480, graphics::window_resize);
        m_window.run();
        m_window.close();
    }
private:
    window_win32& m_window;
};

int main()
{
    graphics::initialize_fonts();

    graphics::window_surface surf("h..");
    window_surface_target surf_target(surf);

    window_win32 xwin(surf_target);
    xwindow_display xwin_display(xwin);
    surf.attach_window(&xwin_display);

    graphics::plot p(true);
    agg::rect_d lim(-1.0, 0.0, 1.0, 10.0);
    p.set_limits(lim);
    p.set_axis_labels_angle(graphics::x_axis, 3.141592 / 4);
    p.enable_label_format(graphics::x_axis, "%.6f");

    draw::path ln;
    agg::path_storage& l = ln.self();
    l.move_to(-0.5, 0.0);
    l.line_to(-0.5, 8.0);
    l.line_to(0.5, 4.0);
    l.close_polygon();

    agg::rgba8 none(0,0,0,0);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 yellow(245, 254, 0, 255);

    trans::scaling_a* trln = new trans::scaling_a(&ln);
    sg_element lne(trln, yellow, red, 2.5, ELEMENT_FILL|ELEMENT_STROKE);
    p.add(lne);

    p.commit_pending_draw();

    int index = surf.attach(&p, "1");

    xwindow_thread thread(xwin);
    thread.start();

    Sleep(4 * 1000);

    draw::path ln2;
    agg::path_storage& l2 = ln2.self();
    l2.move_to(0.8, 1.0);
    l2.line_to(0.8, 7.0);
    l2.line_to(0.3, 4.0);
    l2.close_polygon();

    trans::scaling_a* trln2 = new trans::scaling_a(&ln2);
    sg_element ln2e(trln2, none, blue, 2.5);
    p.add(ln2e);

    surf.slot_refresh(index);
    p.commit_pending_draw();

    for (;;) {
        fprintf(stderr, "sleeping...\n");
        fflush(stderr);
        Sleep(2 * 1000);
    }

    return 0;
}
