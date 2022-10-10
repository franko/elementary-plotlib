// public declarations header.
#include "elem/elem_cpp.h"
#include "elem/elem_c.h"

// the following are private headers.
#include "elem_plot.h"
#include "elem_window.h"
#include "global_elem_window_factory.h"
#include "plot.h"
#include "plot_agent.h"
#include "status_notifier.h"
#include "window_platform_sdl.h"

namespace elem {

void Plot::Show(unsigned width, unsigned height, unsigned flags) {
    plot_impl_->show(width, height, flags);
}

void Plot::Wait() {
    plot_impl_->wait_show_window();
}

Window::Window() : window_impl_(global_elem_window_factory->create()) {
}

Window::Window(const char *layout) : window_impl_{global_elem_window_factory->create_with_layout(layout)} {
}

Window::Window(elem_window *window_impl_) : window_impl_(window_impl_) {
}

Window::~Window() {
    window_impl_->release();
}

void Window::SetLayout(const char *fmt) {
    window_impl_->set_layout(fmt);
}

int Window::Attach(Plot& plot, const char* slot_str) {
    return elem_window_attach(window_impl_, plot.plot_impl_, slot_str);
}

void Window::SlotRefresh(unsigned index) {
    window_impl_->slot_refresh(index);
}

void Window::Start(unsigned width, unsigned height, unsigned flags) {
    window_impl_->start(width, height, flags);
}

void Window::Wait() {
    window_impl_->wait();
}

void Window::Close() {
    window_impl_->close();
}

static void run_user_main(int (*user_main)(), status_notifier<task_status> *initialization, status_notifier<task_status> *main_task) {
    initialization->wait_for_status(kTaskComplete);
    // FIXME: verify if the initialization was successful.
    delete initialization;
    if (elem_window_sdl::initialization_success()) {
        user_main();
        elem_window_sdl::send_quit_event();
    }
    main_task->set(kTaskComplete);
}

int InitializeAndRun(int (*user_main)()) {
    auto initialization = new status_notifier<task_status>();
    auto main_task = new status_notifier<task_status>();
    std::thread events_thread(run_user_main, user_main, initialization, main_task);
    events_thread.detach();
    int status = elem_window_sdl::run_event_loop(initialization);
    main_task->wait_for_status(kTaskComplete);
    return status;
}
}
