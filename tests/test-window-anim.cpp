#include <cmath>
#include <random>

#include "elem/elem_utils.h"
#include "elem/elem.h"

using namespace elem;

void Rotate(const double a, double& x, double& y) {
    double x_tmp = x * std::cos(a) + y * std::sin(a);
    y = -x * std::sin(a) + y * std::cos(a);
    x = x_tmp;
}

void AddTriangle(Plot& plot, double x, double y, double size, double angle, Color fill_color) {
    double x1 = -size / 2, y1 = -size / (2 * std::sqrt(3.0));
    double x2 =  size / 2, y2 = -size / (2 * std::sqrt(3.0));
    double x3 = 0, y3 = size / std::sqrt(3.0);

    Rotate(angle, x1, y1);
    Rotate(angle, x2, y2);
    Rotate(angle, x3, y3);

    Polygon line{{x + x1, y + y1}, {x + x2, y + y2}, {x + x3, y + y3}};
    plot.Add(line, color::Black, 1.0, fill_color, property::Fill | property::Stroke);
}

int main() {
    InitializeFonts();

    Plot plot(Plot::ShowUnits);
    plot.SetLimits(0.0, 0.0, 100.0, 100.0);

    Window window("h.");
    window.Attach(plot, "1");
    window.Start(640, 640, WindowResize);

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_real_distribution<double> unif_dist(20.0, 80.0);
    std::normal_distribution<double> size_dist(20.0, 6.0);
    std::normal_distribution<double> angle_dist(0.0, 2 * 3.14159265358979323846);
    std::uniform_int_distribution<int> randi(0, 255);

    for (int k = 0; k < 30; k++) {
        plot.PushLayer();
        for (int i = 0; i < 40; i++) {
            double x = unif_dist(e1), y = unif_dist(e1), size = size_dist(e1), angle = angle_dist(e1);
            Color color = color::RGB(randi(e1), randi(e1), randi(e1));
            AddTriangle(plot, x, y, size, angle, color);
        }
        if (k+1 < 30) {
            plot.PopLayer();
        }
    }
    window.Wait();
    return 0;
}
