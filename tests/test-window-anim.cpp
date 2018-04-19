#include <cmath>
#include <random>

#include "window.h"
#include "path.h"

void rotate(const double a, double& x, double& y) {
    double x_tmp = x * std::cos(a) + y * std::sin(a);
    y = -x * std::sin(a) + y * std::cos(a);
    x = x_tmp;
}

void
add_triangle(graphics::plot& p, double x, double y, double size, double angle, agg::rgba8 color) {
    double x1 = -size / 2, y1 = -size / (2 * std::sqrt(3.0));
    double x2 =  size / 2, y2 = -size / (2 * std::sqrt(3.0));
    double x3 = 0, y3 = size / std::sqrt(3.0);

    rotate(angle, x1, y1);
    rotate(angle, x2, y2);
    rotate(angle, x3, y3);

    agg::rgba8 black(0, 0, 0, 255);
    auto line = new graphics::path(color, black, 1.0, ELEMENT_FILL|ELEMENT_STROKE);
    line->move_to(x + x1, y + y1);
    line->line_to(x + x2, y + y2);
    line->line_to(x + x3, y + y3);
    line->close_polygon();
    p.add(line);
}

int main()
{
    graphics::initialize_fonts();

    graphics::window win("h.");

    graphics::plot p(graphics::plot::show_units);
    agg::rect_d lim(0.0, 0.0, 100.0, 100.0);
    p.set_limits(lim);
    int index = win.attach(&p, "1");

    win.start(640, 640, graphics::window_resize);
    sleep(1);

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_real_distribution<double> unif_dist(20.0, 80.0);
    std::normal_distribution<double> size_dist(20.0, 6.0);
    std::normal_distribution<double> angle_dist(0.0, 2 * 3.14159265358979323846);
    std::uniform_int_distribution<int> randi(0, 255);

    for (int k = 0; k < 30; k++) {
        p.push_layer();
        for (int i = 0; i < 40; i++) {
            double x = unif_dist(e1), y = unif_dist(e1), size = size_dist(e1), angle = angle_dist(e1);
            agg::rgba8 color(randi(e1), randi(e1), randi(e1), 255);
            add_triangle(p, x, y, size, angle, color);
            win.slot_refresh(index);
            p.commit_pending_draw();
        }
        p.pop_layer();
    }

    return 0;
}
