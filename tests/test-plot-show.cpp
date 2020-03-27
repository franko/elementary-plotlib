#include <cmath>

#include "elem/elem.h"

using namespace elem;

int main() {
    Plot plot;
    const double x0 = 0.0001, x1 = 8 * math::Tau();
    plot.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);
    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");
    plot.Show(640, 480, WindowResize);
    plot.AddStroke(FxLine(0.8, x1, [](double x) { return std::cos(x) / x; }), color::Red, 1.5);
    plot.Wait();
    return 0;
}
