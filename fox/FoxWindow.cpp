#include <thread>
#include <cstring>

#include "libcanvas.h"
#include "fox/GraphicsWindow.h"

void RunFoxWindow(FXApp *app, FXMainWindow *win, unsigned width, unsigned height) {
    app->create();
    win->resize(width, height);
    win->show(PLACEMENT_SCREEN);
    app->run();
}

using libcanvas::Plot;
using libcanvas::Window;

class FoxWindow {
public:
    FoxWindow(int& argc, char **argv, const char *split_str) : app_("libcanvas", "libcanvas") {
        app_.init(argc, argv);
        main_window_ = new FXMainWindow(&app_, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 0, 0);
        graphics_window_ = new GraphicsWindow(main_window_, split_str, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    }

    int Attach(Plot& p, const char *slot_str) {
        return graphics_window_->Attach(p, slot_str);
    }

    void SlotRefresh(unsigned index) {
        graphics_window_->SlotRefresh(index);
    }

    // FIXME: do not ignore "flags"
    void Start(unsigned width, unsigned height, unsigned flags) {
        std::thread wt(RunFoxWindow, &app_, main_window_, width, height);
        wt.detach();
    }

    void Wait() {
        graphics_window_->Wait();
    }

private:
    FXApp app_;
    FXMainWindow *main_window_;
    GraphicsWindow *graphics_window_;
};

Window::Window() {
    int argc = 1;
    char argv0[32];
    char *argv[] = {argv0, 0};
    memcpy(argv0, "libcanvas", 16);
    window_impl_ = (WindowImpl *) new FoxWindow(argc, argv, nullptr);
}

Window::Window(const char *split_str) {
    int argc = 1;
    char argv0[32];
    char *argv[] = {argv0, 0};
    memcpy(argv0, "libcanvas", 10);
    window_impl_ = (WindowImpl *) new FoxWindow(argc, argv, split_str);
}

Window::~Window() {
    FoxWindow *win = (FoxWindow *) window_impl_;
    delete win;
}

int Window::Attach(Plot& plot, const char* slot_str) {
    FoxWindow *win = (FoxWindow *) window_impl_;
    return win->Attach(plot, slot_str);
}

void Window::SlotRefresh(unsigned index) {
    FoxWindow *win = (FoxWindow *) window_impl_;
    win->SlotRefresh(index);
}

void Window::Start(unsigned width, unsigned height, unsigned flags) {
    FoxWindow *win = (FoxWindow *) window_impl_;
    win->Start(width, height, flags);
}

void Window::Wait() {
    FoxWindow *win = (FoxWindow *) window_impl_;
    win->Wait();
}
