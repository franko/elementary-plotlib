#include <cmath>
#include <cstdio>

#include "libcanvas.h"

using namespace libcanvas;

template <typename Function>
class FunctionPath : public VertexSource {
public:
    FunctionPath(Function f, double x0, double x1, int n) : m_function(f), m_x0(x0), m_x1(x1), m_points_number(n) {
    }

    virtual void rewind(unsigned path_id) {
        m_index = 0;
    }

    virtual unsigned vertex(double *x, double *y) {
        if (m_index <= m_points_number + 1) {
            const double x_current = m_x0 + m_index * (m_x1 - m_x0) / (m_points_number + 1);
            *x = x_current;
            *y = m_function(x_current);
            fprintf(stderr, "%d %g %g\n", m_index, *x, *y);
            return (m_index++ == 0 ? command::MoveTo : command::LineTo);
        }
        return command::Stop;
    }
 
private:
    Function m_function;
    double m_x0, m_x1;
    int m_index, m_points_number;
};

int main() {
    InitializeFonts();

    Window window;
    Plot plot(Plot::ShowUnits | Plot::AutoLimits);
    plot.SetClipMode(false);

    auto f = [](double x) { return std::sin(x) / x; };
    FunctionPath<decltype(f)> f_source(f, 0.001, 8 * 2 *  3.14159265358979323846, 128);
    CustomPath f_path{f_source};
    plot.Add(f_path, color::Blue, 1.5, color::None, property::Stroke);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");

    plot.CommitPendingDraw();

    window.Attach(plot, "");
    window.Start(640, 480, WindowResize);
    window.Wait();
    return 0;
}
