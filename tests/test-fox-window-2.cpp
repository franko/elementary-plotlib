#include "fox/window_fox.h"
#include "fox/GraphicsWindow.h"
#include "debug_log.h"
#include "path.h"

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
    auto window = new GraphicsWindow(main_window, nullptr, LAYOUT_FILL_X|LAYOUT_FILL_Y);

    graphics::plot p(graphics::plot::show_units);
    p.set_limits({-1.0, 0.0, 1.0, 10.0});
    p.set_axis_labels_angle(graphics::x_axis, 3.141592 / 4);
    p.enable_label_format(graphics::x_axis, "%.6f");

    agg::rgba8 none(0,0,0,0);
    agg::rgba8 red(180, 0, 0, 255);
    agg::rgba8 blue(0, 0, 180, 255);
    agg::rgba8 yellow(245, 254, 0, 255);

    auto line = new graphics::polygon {{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    p.add(line, red, 2.5, yellow, graphics::property::fill | graphics::property::stroke);

    p.commit_pending_draw();

    int index = window->attach(&p, "");

    std::thread wt(run_fox, app, main_window);
    wt.detach();

    sleep(4);

    auto line2 = new graphics::polygon {{0.8, 1.0}, {0.8, 7.0}, {0.3, 4.0}};
    p.add(line2, blue, 2.5, none);

    window->slot_refresh(index);
    p.commit_pending_draw();

    window->wait();

    return 0;
}
