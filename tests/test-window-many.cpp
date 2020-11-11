#include <array>
#include <cmath>
#include <cstdio>
#include <random>

#include "elem/elem.h"
#include "elem/elem_utils.h"

using namespace elem;

int main() {
    InitializeFonts();

    Plot plot;
    const double x0 = 0.0001, x1 = 8 * math::Tau();
    plot.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);

    plot.SetTitle("Function plot example");
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");

    const int n = 50;
    std::array<int, n> permutation;
    std::array<Window, n> windows_list;
    for (int i = 0; i < n; i++) {
        Window window;
        window.Attach(plot, "");
        window.Start(640, 480, WindowResize);
        permutation[i] = i;
        windows_list[i] = std::move(window);
    }

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, 1000 * n);

    int i;
    for (i = 0; i <= n - 2; i++) {
        int j = i + (distrib(gen) % (n - i));
        int tmp = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = tmp;
    }

    utils::Sleep(2);
    printf("closing the windows...\n");
    fflush(stdout);

    for (int i = 0; i < n; i++) {
        printf("closing window %d\n", permutation[i]);
        fflush(stdout);
        windows_list[permutation[i]].Close();
    }

    return 0;
}

