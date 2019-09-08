#include <thread>
#include <cstring>

#include "FXLibcanvasWindow.h"

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
    FoxWindow(const char *split_str) : app_("libcanvas", "libcanvas") {
        argc_ = 1;
        strcpy(argv0_, "libcanvas");
        argv_[0] = argv0_;
        argv_[1] = nullptr;

        app_.init(argc_, argv_);
        main_window_ = new FXMainWindow(&app_, "Graphics Window", nullptr, nullptr, DECOR_ALL, 0, 0, 0, 0);
        libcanvas_window_ = new LibcanvasWindow(main_window_, split_str, LAYOUT_FILL_X|LAYOUT_FILL_Y);
        main_window_->setTarget(this);
    }

    bool isRunning() const {
        return libcanvas_window_ != nullptr;
    }

    int Attach(Plot& p, const char *slot_str) {
        if (isRunning()) {
            return libcanvas_window_->Attach(p, slot_str);
        }
        return -1;
    }

    void SlotRefresh(unsigned index) {
        if (isRunning()) {
            libcanvas_window_->SlotRefresh(index);
        }
    }

    // FIXME: do not ignore "flags"
    void Start(unsigned width, unsigned height, unsigned flags) {
        std::thread wt(RunFoxWindow, &app_, main_window_, width, height);
        wt.detach();
    }

    void Wait() {
        if (isRunning()) {
            libcanvas_window_->Wait();
        }
    }

    long onClose(FXObject *, FXSelector, void *);

private:
    int argc_;
    char argv0_[16];
    char *argv_[2];

    FXApp app_;
    FXMainWindow *main_window_;
    LibcanvasWindow *libcanvas_window_;
};

FXDEFMAP(FoxWindow) FoxWindowMap[] = {
    FXMAPFUNC(SEL_CLOSE, 0, FoxWindow::onClose),
};

FXIMPLEMENT(FoxWindow,FXObject,FoxWindowMap,ARRAYNUMBER(FoxWindowMap))

long FoxWindow::onClose(FXObject *, FXSelector, void *) {
    // We set the main window's and graphics window's pointer to null to mean
    // that the windows is closed.
    main_window_ = nullptr;
    libcanvas_window_ = nullptr;
    return 0;
}

Window::Window() {
    window_impl_ = (WindowImpl *) new FoxWindow(nullptr);
}

Window::Window(const char *split_str) {
    window_impl_ = (WindowImpl *) new FoxWindow(split_str);
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
