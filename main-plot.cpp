#include "plot.h"
#include "path.h"

int main()
{
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

    return 0;
}
