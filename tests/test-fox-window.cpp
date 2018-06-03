#include "fox/window_fox.h"
#include "fox/PlotCanvas.h"
#include "debug_log.h"
#include "path.h"

template <typename Function>
void add_function(graphics::plot& p, double x0, double x1, Function f, agg::rgba8 color, int n = 512) {
    agg::rgba8 none(0,0,0,0);
    auto line = new graphics::path();
    line->move_to(x0, f(x0));
    for (int i = 1; i <= n; i++) {
        const double x = x0 + i * (x1 - x0) / n;
        line->line_to(x, f(x));
    }
    p.add(line, color, 1.5, none, graphics::property::stroke);
}

void run_fox(FXApp *app, FXMainWindow *win) {
    app->create();
    win->show(PLACEMENT_SCREEN);
    app->run();
    debug_log("thread terminating...");
    delete app;
    debug_log("app terminated...");
}

int main(int argc, char *argv[]) {
    graphics::initialize_fonts();

    auto app = new FXApp("libcanvas", "libcanvas");
    app->init(argc, argv);

    auto main_window = new FXMainWindow(app, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 640, 480);
    auto canvas = new PlotCanvas(main_window, nullptr, PlotCanvas::ID_CANVAS, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN);
    canvas->setTarget(canvas);

    graphics::window_surface surface(nullptr);
    window_fox win(surface, canvas);
    surface.attach_window(&win);

    graphics::plot p(graphics::plot::show_units | graphics::plot::auto_limits);
    p.set_clip_mode(false);

    agg::rgba8 blue(0, 0, 180, 255);

    const double x0 = 0.0001, x1 = 8 * 2 *  3.14159265358979323846;
    add_function(p, x0, x1, [](double x) { return std::sin(x) / x; }, blue);

    p.set_title("Function plot example");
    p.set_x_axis_title("x variable");

    p.commit_pending_draw();

    surface.attach(&p, "");

    std::thread wt(run_fox, app, main_window);
    wt.detach();

    notify_request<graphics::window_status_e> req{graphics::window_closed};
    int retval = win.set_notify_request(req);
    if (retval == request_success) {
        req.wait();
    }
    return 0;
}
