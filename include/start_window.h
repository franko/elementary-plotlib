#pragma once

/* The functions below are meant to be used with a Window type that:
   - inherits from display_window_status for the methods lock, unlock
     and wait_until_notification
   - implements the "start_blocking" method.

   The purpose of the start_window_new_thread function is to start a
   detached thread to run the window without blocking the calling
   thread.
*/

template <typename Window>
void run_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    window->start_blocking(width, height, flags);
    window->unlock();
}

template <typename Window>
int start_window_new_thread(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    std::thread window_thread(run_window<Window>, window, width, height, flags);
    window->unlock();
    window_thread.detach();
    request_error_e status = window->wait_until_notification(graphics::window_running);
    if (status == request_satisfied || status == request_success) {
        return 0;
    }
    return 1;
}
