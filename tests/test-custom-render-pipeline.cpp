#include <cmath>
#include <random>

#include "elem_plot.h"
#include "elem_window.h"
#include "global_elem_window_factory.h"
#include "trans.h"
#include "sg_path.h"
#include "window_flags.h"

/* This example demonstrate how to build a custom rendering pipeline by
   chaining objects and transforms. In this case we perform a dash transform
   to obtain a dashed line and then we perform a stroking with a large width.
   It should be noted that all the transforms are done before the scaling
   object that closes the chains. The final scaling object transforms the
   model coordinates in screen coordinates. Therefore the dash and stroking's
   widths and lengths are expressed in the model's coordinates.

   It is possible to add also transforms *after* the scaling object working
   therefore in screen coordinates. This is actually the more usual way to use
   dash and stroke transforms. For example the length of the dash is more
   naturally given in pixel and therefore in screen coordinates.

   As for the classes actually used, below we are using objects from the
   graphics::transform namespace and they all works by implementing the
   elem_object interface with virtual functions. This is the more simple
   approach but in principle one could build a chain using only basic AGG
   objects, without virtual methods or interfaces. The only constraint is that
   the class itself has to implement the elem_object interface. In this case we
   have it automatically because we are inheriting from
   graphics::transform::scaling_a.

   Note also that we are using object from graphics::transform with the "_a"
   suffix. The objects with "_a" does not attempt to delete the pointer to the
   object they get which is the appropriate thing in this case.
*/
class custom_render_path : public graphics::transform::scaling_a {
public:
    custom_render_path():
        graphics::transform::scaling_a(&m_stroke), m_stroke(&m_dash), m_dash(&m_path), m_path()
    {
        /* We define here how we want the stroke transform. */
        m_stroke.self().line_cap(round_cap);
        m_stroke.self().line_join(round_join);
    }

    /* The methods below are required as the underlying objects of the rendering
       chain are not directly accessible, they are private members of the class. */
    agg::path_storage& path_handle() { return m_path.self(); }
    void set_stroke_width(double w) { m_stroke.width(w); }
    void add_dash_length(double a, double b) { m_dash.add_dash(a, b); }

private:
    graphics::transform::stroke_a m_stroke;
    graphics::transform::dash_a m_dash;
    sg_path m_path;
};

int main()
{
    graphics::initialize_fonts();

    elem_window *win = global_elem_window_factory->create();
    elem_plot p(graphics::plot::show_units);
    p.set_limits({-0.7, -0.3, 0.7, 1.1});

    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 yellow(255, 242, 0, 255);
    agg::rgba8 none(0,0,0,0);

    auto obj = new custom_render_path{};
    obj->path_handle().move_to(-0.5, 0.0);
    obj->path_handle().line_to(+0.5, 0.0);
    obj->path_handle().line_to(0.0, std::sqrt(3.0)/2);
    obj->path_handle().close_polygon();

    obj->set_stroke_width(0.05);
    obj->add_dash_length(0.18, 0.1);

    p.add(obj, blue, 1.5, yellow, graphics::property::stroke|graphics::property::fill);

    p.set_title("Custom render pipeline example");

    p.commit_pending_draw();

    win->attach(&p, "");
    win->start(640, 640, graphics::window_resize);
    win->wait();

    return 0;
}
