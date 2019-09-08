#include <cmath>
#include <random>

#include "FXLibcanvasWindow.h"

using namespace libcanvas;

class PlotExampleApp : public FXApp {
    FXDECLARE(PlotExampleApp)
public:
    PlotExampleApp();

    void addPlot(Plot p);

    enum {
        ID_PLOT_ADD = FXApp::ID_LAST,
        ID_LAST,
    };

    void create() override;
    long onPlotAdd(FXObject *, FXSelector, void *);

private:
    Plot m_plot;
    FXMainWindow *m_main_window;
    LibcanvasWindow *m_graphics_window;

    std::random_device m_rd;
    std::default_random_engine m_re;
};

FXDEFMAP(PlotExampleApp) PlotExampleAppMap[]={
    FXMAPFUNC(SEL_COMMAND, PlotExampleApp::ID_PLOT_ADD, PlotExampleApp::onPlotAdd),
};

FXIMPLEMENT(PlotExampleApp,FXApp,PlotExampleAppMap,ARRAYNUMBER(PlotExampleAppMap))

PlotExampleApp::PlotExampleApp(): FXApp("libcanvas", "libcanvas"), m_re(m_rd()) {
    m_main_window = new FXMainWindow(this, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 640);
    auto vframe = new FXVerticalFrame(m_main_window, FRAME_SUNKEN|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 10, 10);
    m_graphics_window = new LibcanvasWindow(vframe, nullptr, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    new FXHorizontalSeparator(vframe, SEPARATOR_GROOVE|LAYOUT_FILL_X);
    new FXButton(vframe, "Add", nullptr, this, ID_PLOT_ADD, FRAME_THICK|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
}

void PlotExampleApp::create() {
    FXApp::create();
    m_main_window->show(PLACEMENT_SCREEN);
}

static Path RotatedSquare(double x, double y, double h, double angle) {
    const double c = std::cos(angle), s = std::sin(angle);
    return Polygon{
        {x + h * (-c - s), y + h * ( s - c)},
        {x + h * ( c - s), y + h * (-s - c)},
        {x + h * ( c + s), y + h * (-s + c)},
        {x + h * (-c + s), y + h * ( s + c)}};
}

long PlotExampleApp::onPlotAdd(FXObject *, FXSelector, void *) {
    std::normal_distribution<double> nrnd(0.0, 1.0);
    std::uniform_real_distribution<double> urnd(0.0, 2 * 3.14159265359);
    m_plot.Add(RotatedSquare(nrnd(m_re) * 0.2, nrnd(m_re) * 0.2, nrnd(m_re) * 0.15, urnd(m_re)), color::Gray, 1.0, color::None);
    return 1;   
}

void PlotExampleApp::addPlot(Plot p) {
    m_plot = std::move(p);
    m_graphics_window->Attach(m_plot, "");
}

Plot CreatePlot() {
    Plot p{Plot::ShowUnits};
    p.SetLimits({-1.0, -1.0, 1.0, 1.0});
    Polygon line{{0.0, -0.5}, {0.0, 0.5}, {0.8, 0.0}};
    p.Add(line, color::Red, 2.5, color::Yellow, property::Fill | property::Stroke);
    return p;
}

int main(int argc, char *argv[]) {
    InitializeFonts();
    PlotExampleApp app;
    app.init(argc, argv);
    app.addPlot(CreatePlot());
    app.create();
    return app.run();
}
