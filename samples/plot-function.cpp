#include <cmath>

#include "elem/elem.h"

using namespace elem;

int main() {
    // The function FxLine generate a Path object based on a
    // mathematical function (x) -> f(x).
    Path sin_line = FxLine(0.0001, 8 * math::Tau(), [](double x) { return std::sin(x) / x; });

    // Declare a plot object.
    Plot plot;
    // Add the line just created with a blue color and 1.5 pixel
    // line's width.
    plot.AddStroke(sin_line, color::Blue, 1.5);

    // Set Plot titles.
    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");

    // Show the plot on the screen.
    plot.Show(640, 480, WindowResize);

    // Add one more line corresponding to a different function.
    // Note that objects can be added after the plot is already
    // shown on the screen.
    Path cos_line = FxLine(0.8, 8 * math::Tau(), [](double x) { return std::cos(x) / x; });
    plot.AddStroke(cos_line, color::Red, 1.5);

    // Wait until the window showing the plot is closed.
    plot.Wait();
    return 0;
}
