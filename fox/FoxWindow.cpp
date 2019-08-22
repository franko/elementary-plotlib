#include <thread>
#include <cstring>

#include "libcanvas.h"
#include "fox/GraphicsWindow.h"

// the following are private headers.
#include "plot_agent.h"

using namespace libcanvas;

void RunFoxWindow(FXApp *app, FXMainWindow *win, unsigned width, unsigned height) {
    app->create();
    win->resize(width, height);
    win->show(PLACEMENT_SCREEN);
    app->run();
}

class FoxWindow : public FXObject {
    FXDECLARE(FoxWindow)
protected:
    FoxWindow() { }
private:
    FoxWindow(const FoxWindow&);
    FoxWindow &operator=(const FoxWindow&);
public:
    FoxWindow(int& argc, char **argv, const char *split_str) : app_("libcanvas", "libcanvas") {
        app_.init(argc, argv);
        main_window_ = new FXMainWindow(&app_, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 0, 0);
        graphics_window_ = new GraphicsWindow(main_window_, split_str, LAYOUT_FILL_X|LAYOUT_FILL_Y);
        main_window_->setTarget(this);
    }

    bool isRunning() const {
        return graphics_window_ != nullptr;
    }

    int Attach(Plot& p, const char *slot_str) {
        if (isRunning()) {
            return graphics_window_->Attach(p, slot_str);
        }
        return -1;
    }

    void SlotRefresh(unsigned index) {
        if (isRunning()) {
            graphics_window_->SlotRefresh(index);
        }
    }

    // FIXME: do not ignore "flags"
    void Start(unsigned width, unsigned height, unsigned flags) {
        std::thread wt(RunFoxWindow, &app_, main_window_, width, height);
        wt.detach();
    }

    void Wait() {
        if (isRunning()) {
            graphics_window_->Wait();
        }
    }

    long onClose(FXObject *, FXSelector, void *);
private:
    FXApp app_;
    FXMainWindow *main_window_;
    GraphicsWindow *graphics_window_;
};

FXDEFMAP(FoxWindow) FoxWindowMap[] = {
    FXMAPFUNC(SEL_CLOSE, 0, FoxWindow::onClose),
};

FXIMPLEMENT(FoxWindow,FXObject,FoxWindowMap,ARRAYNUMBER(FoxWindowMap))

long FoxWindow::onClose(FXObject *, FXSelector, void *) {
    // We set the main window's and graphics window's pointer to null to mean
    // that the windows is closed.
    main_window_ = nullptr;
    graphics_window_ = nullptr;
    return 0;
}

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
    int index = win->Attach(plot, slot_str);
    plot.plot_agent_->AddWindow(this, index);
    return index;
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
