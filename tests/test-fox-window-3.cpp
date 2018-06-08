#include <cmath>
#include <random>

#include "fox/window_fox.h"
#include "fox/GraphicsWindow.h"
#include "debug_log.h"
#include "path.h"

class PlotExampleApp : public FXApp {
    FXDECLARE(PlotExampleApp)
public:
    PlotExampleApp();

    virtual ~PlotExampleApp();

    void addPlot(graphics::plot *p);

    enum {
        ID_PLOT_ADD = FXApp::ID_LAST,
        ID_LAST,
    };

    void create() override;

    long onPlotAdd(FXObject *, FXSelector, void *);

private:
    graphics::plot *m_plot;
    int m_plot_index;
    FXMainWindow *m_main_window;
    GraphicsWindow *m_graphics_window;

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
    m_graphics_window = new GraphicsWindow(vframe, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    new FXHorizontalSeparator(vframe, SEPARATOR_GROOVE|LAYOUT_FILL_X);
    new FXButton(vframe, "Add", nullptr, this, ID_PLOT_ADD, FRAME_THICK|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
}

PlotExampleApp::~PlotExampleApp() {
    delete m_plot;
}

void PlotExampleApp::create() {
    FXApp::create();
    m_main_window->show(PLACEMENT_SCREEN);
}

static graphics::path *rotated_square(double x, double y, double h, double angle) {
    const double c = std::cos(angle), s = std::sin(angle);
    auto line = new graphics::path;
    line->move_to(x + h * (-c - s), y + h * ( s - c));
    line->line_to(x + h * ( c - s), y + h * (-s - c));
    line->line_to(x + h * ( c + s), y + h * (-s + c));
    line->line_to(x + h * (-c + s), y + h * ( s + c));
    line->close_polygon();
    return line;
}

long PlotExampleApp::onPlotAdd(FXObject *, FXSelector, void *) {
    agg::rgba8 none(0, 0, 0, 0);
    agg::rgba8 gray(50, 50, 50, 255);
    std::normal_distribution<double> nrnd(0.0, 1.0);
    std::uniform_real_distribution<double> urnd(0.0, 2 * 3.14159265359);
    m_plot->add(rotated_square(nrnd(m_re) * 0.2, nrnd(m_re) * 0.2, nrnd(m_re) * 0.15, urnd(m_re)), gray, 1.0, none);
    m_graphics_window->slot_refresh(m_plot_index);
    m_plot->commit_pending_draw();
    return 1;   
}

void PlotExampleApp::addPlot(graphics::plot *p) {
    m_plot = p;
    m_plot_index = m_graphics_window->attach(p, "");
}

graphics::plot *create_plot() {
    auto p = new graphics::plot(graphics::plot::show_units);
    p->set_limits({-1.0, -1.0, 1.0, 1.0});

    agg::rgba8 none(0, 0, 0, 0);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 yellow(245, 254, 0, 255);

    auto line = new graphics::polygon {{0.0, -0.5}, {0.0, 0.5}, {0.8, 0.0}};
    p->add(line, red, 2.5, yellow, graphics::property::fill | graphics::property::stroke);
    p->commit_pending_draw();
    return p;    
}

int main(int argc, char *argv[]) {
    graphics::initialize_fonts();
    PlotExampleApp app{};
    app.init(argc, argv);
    auto plot = create_plot();
    app.addPlot(plot);
    app.create();
    return app.run();
}
