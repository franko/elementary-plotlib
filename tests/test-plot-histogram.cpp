#include <random>

#include "elem/elem.h"

using namespace elem;

template <typename ParentDistDensity, typename RandomEngine>
void GenHistogram(Plot& plot, RandomEngine &e, ParentDistDensity parent_df, double x1, double x2, int bins_number, int N) {
    const double dx = (x2 - x1) / bins_number;
    for (int bin = 0; bin < bins_number; bin++) {
        double x_bin = x1 + dx * (bin + 0.5);
        double x_bin_inf = x1 + dx * bin, x_bin_sup = x1 + dx * (bin + 1);
        std::poisson_distribution<int> poisson_dist(parent_df(x_bin) * dx * N);
        double count = poisson_dist(e);
        Polygon bar{{x_bin_inf, 0.0}, {x_bin_sup, 0.0}, {x_bin_sup, count}, {x_bin_inf, count}};
        plot.Add(bar, color::Black, 1.0, color::Green, property::Fill | property::Crisp | property::Outline);
    }
}

int main() {
    InitializeFonts();

    Plot plot;
    plot.SetTitle("Gaussian distribution example");

    std::random_device r;
    std::default_random_engine e1(r());
    const double gauss_x0 = 4.5, gauss_sigma = 0.5;
    const int n_samples = 1000, n_bins = 16;
    const double x0 = gauss_x0 - 5 * gauss_sigma, x1 = gauss_x0 + 5 * gauss_sigma;
    auto normal_density = [=](double x) { return 1 / (sqrt(math::Tau()) * gauss_sigma) * std::exp(- 0.5 * std::pow((x - gauss_x0) / gauss_sigma, 2)); };
    GenHistogram(plot, e1, normal_density, x0, x1, n_bins, n_samples);

    Path line = FxLine(x0, x1, [&](double x) { return normal_density(x) * (x1 - x0) / n_bins * n_samples; });
    plot.AddStroke(std::move(line), color::Red, 1.5);

    Window window;
    window.Attach(plot, "");

    window.Start(800, 480, WindowResize);
    window.Wait();
    return 0;
}
