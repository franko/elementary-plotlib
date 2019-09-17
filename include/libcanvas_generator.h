#ifndef LIBCANVAS_GENERATOR_H_
#define LIBCANVAS_GENERATOR_H_

namespace libcanvas {

template <typename Function, typename PathType = Path>
void FxLineDraw(PathType& line, double x0, double x1, Function f, int sampling_intervals = 128) {
    line.MoveTo(x0, f(x0));
    for (int i = 1; i <= sampling_intervals; i++) {
        const double x = x0 + i * (x1 - x0) / sampling_intervals;
        line.LineTo(x, f(x));
    }
}

template <typename Function, typename PathType = Path>
PathType FxLine(double x0, double x1, Function f, int sampling_intervals = 128) {
    PathType line;
    FxLineDraw(line, x0, x1, f, sampling_intervals);
    return line;
}

}
#endif
