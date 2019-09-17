#ifndef LIBCANVAS_GENERATOR_H_
#define LIBCANVAS_GENERATOR_H_

namespace libcanvas {

template <typename Function>
Path FxLine(double x0, double x1, Function f, int sampling_intervals = 128) {
    Path line;
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= sampling_intervals; i++) {
        const double x = x0 + i * (x1 - x0) / sampling_intervals;
        line.LineTo(x, f(x));
    }
    return line;
}

}
#endif
