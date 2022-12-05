#ifdef WIN32
  #include <windows.h>
#endif
#include <stdint.h>

#include "render_config.h"
#include "sdl/window_sdl.h"

#include "complete_notify.h"

struct window_create_message {
    enum { success = 0, window_error };

    const char *caption;
    unsigned width;
    unsigned height;
    unsigned flags;
    int return_code;

    window_sdl *this_window;
    window_close_callback *callback;
};

bool window_sdl::g_sdl_initialized = false;
Uint32 window_sdl::g_user_event_type = -1;
int window_sdl::g_sdl_init_status = -1;
std::mutex window_sdl::g_register_mutex;
agg::pod_bvector<window_entry> window_sdl::g_window_entries;

window_sdl::window_sdl(graphics::window_surface& window_surface):
    m_window(nullptr), m_renderer(nullptr), m_texture(nullptr),
    m_window_surface(window_surface)
{
}

static Uint32 config_sdl_pixel_format() {
    switch (graphics::pixel_format) {
        case agg::pix_format_bgr24:
            return SDL_PIXELFORMAT_BGR24;
        case agg::pix_format_rgb24:
            return SDL_PIXELFORMAT_RGB24;
        case agg::pix_format_bgra32:
            return SDL_PIXELFORMAT_BGRA32;
        case agg::pix_format_rgba32:
            return SDL_PIXELFORMAT_RGBA32;
        default:
            /* */;
    }
    return SDL_PIXELFORMAT_UNKNOWN;
}

void window_sdl::process_window_event(SDL_Event *event) {
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN: {
        int w, h, w_pixels, h_pixels;
        SDL_GL_GetDrawableSize(m_window, &w_pixels, &h_pixels);
        SDL_GetWindowSize(m_window, &w, &h);
        m_window_surface.resize(w, h, w_pixels, h_pixels);
        m_window_surface.render();
        set_status(graphics::window_running);
        break;
    }
    case SDL_WINDOWEVENT_RESIZED: {
        int w = event->window.data1, h = event->window.data2;
        int w_pixels, h_pixels;
        SDL_GL_GetDrawableSize(m_window, &w_pixels, &h_pixels);
        setup_renderer(w_pixels, h_pixels);
        m_window_surface.resize(w, h, w_pixels, h_pixels);
        m_window_surface.render();
        break;
    }
    case SDL_WINDOWEVENT_EXPOSED:
        m_window_surface.update_window_area();
        break;
    case SDL_WINDOWEVENT_CLOSE:
        SDL_DestroyTexture(m_texture);
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        set_status(graphics::window_closed);
        // The unregister function below does delete the window but
        // we are in a window's method so the function must be called
        // only before returning.
        unregister_window();
        break;
    default:
        break;
    }
}

void window_sdl::setup_renderer(int w, int h) {
    // We assume the renderer and texture are always either both initialized
    // or null.
    if (m_renderer) {
        SDL_DestroyTexture(m_texture);
        SDL_DestroyRenderer(m_renderer);
    }
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    // We use for the texture the same pixel format of the render_config.h
    // file to match the pixel format used in the window_surface.
    m_texture = SDL_CreateTexture(m_renderer, config_sdl_pixel_format(), SDL_TEXTUREACCESS_STREAMING, w, h);
    assert(m_renderer != nullptr && m_texture != nullptr);
}

void window_sdl::process_update_event() {
    if (!m_update_notify.completed()) {
        m_window_surface.slot_refresh(m_update_notify.message());
        m_update_notify.notify();
    }
}

window_sdl *window_sdl::select_on_window_id(Uint32 window_id) {
    g_register_mutex.lock();
    for (unsigned i = 0; i < g_window_entries.size(); i++) {
        window_entry& we = g_window_entries[i];
        if (we.window && we.window_id == window_id) {
            g_register_mutex.unlock();
            return we.window;
        }
    }
    g_register_mutex.unlock();
    return nullptr;
}

int window_sdl::initialize_sdl() {
#ifdef WIN32
    HINSTANCE lib = LoadLibrary("user32.dll");
    int (*SetProcessDPIAware)() = (int (*)()) GetProcAddress(lib, "SetProcessDPIAware");
    SetProcessDPIAware();
#endif
    g_sdl_init_status = SDL_Init(SDL_INIT_VIDEO);
    if (g_sdl_init_status) {
        fprintf(stderr, "error starting graphical system: %s\n", SDL_GetError());
        return g_sdl_init_status;
    }
    g_user_event_type = SDL_RegisterEvents(1);
    if (g_user_event_type == ((Uint32)-1)) {
        return (-1);
    }
    g_sdl_initialized = true;
    return 0;
}

void window_sdl::event_loop(status_notifier<task_status> *initialization) {
    if (initialize_sdl()) {
        initialization->set(kTaskComplete);
        return;
    }
    initialization->set(kTaskComplete);
    SDL_Event event;
    bool quit = false, user_thread_done = false;
    while (!quit || !user_thread_done) {
        int event_status = SDL_WaitEvent(&event);
        if (event_status == 0) {
            break;
        }
        switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_WINDOWEVENT:
            {
                window_sdl *window = select_on_window_id(event.window.windowID);
                if (window) {
                    window->process_window_event(&event);
                }
                break;
            }
        default:
            if (event.type == window_sdl::g_user_event_type) {
                if (event.user.code == kUpdateRegion) {
                    window_sdl *window = select_on_window_id((intptr_t) event.user.data1);
                    if (window) {
                        window->process_update_event();
                    }
                } else if (event.user.code == kCreateWindow) {
                    complete_notify<window_create_message> *create = (complete_notify<window_create_message> *) event.user.data1;
                    window_create_message& message = create->message();
                    Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI | (message.flags & graphics::window_resize ? SDL_WINDOW_RESIZABLE : 0);
#if __APPLE__
                    window_flags |= SDL_WINDOW_METAL;
#else
                    window_flags |= SDL_WINDOW_OPENGL;
#endif
                    // Note hor HiDPI on macOS we are requesting here for a window of a given size,
                    // in points. Later when we will treat the window's shown and resize event
                    // we will retrieve a larger buffer by a HiDPI multiple.
                    SDL_Window *window = SDL_CreateWindow(message.caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, message.width, message.height, window_flags);
                    if (!window) {
                        message.return_code = window_create_message::window_error;
                        create->notify();
                        break;
                    }
                    message.this_window->setup_sdl_window(window);
                    message.this_window->register_window(window, message.callback);
                    message.return_code = window_create_message::success;
                    // On macOS the plotting window will not be raised if we don't
                    // explicitly call the SDL_RaiseWindow function.
                    SDL_RaiseWindow(window);
                    create->notify();
                } else if (event.user.code == kQuitEventLoop) {
                    user_thread_done = true;
                }
            }
        }
    }
}

void window_sdl::setup_sdl_window(SDL_Window *window) {
    m_window = window;
    int w, h;
    SDL_GL_GetDrawableSize(m_window, &w, &h);
    setup_renderer(w, h);
}

void window_sdl::register_window(SDL_Window *window, window_close_callback *close_callback) {
    g_register_mutex.lock();
    g_window_entries.add(window_entry{this, SDL_GetWindowID(window), close_callback});
    g_register_mutex.unlock();
}

void window_sdl::compact_window_register() {
    const unsigned we_size = g_window_entries.size();
    unsigned iw = 0;
    for (unsigned ir = 0; ir < we_size; ir++) {
        if (g_window_entries[ir].window) {
            if (iw < ir) {
                g_window_entries[iw] = g_window_entries[ir];
            }
            iw++;
        }
    }
    g_window_entries.free_tail(iw);
}

void window_sdl::unregister_window() {
    int empty_windows_number = 0;
    g_register_mutex.lock();
    for (unsigned i = 0; i < g_window_entries.size(); i++) {
        window_entry& we = g_window_entries[i];
        if (we.window == this) {
            we.window = nullptr;
            we.window_id = -1;
            we.close_callback->execute();
            delete we.close_callback;
            we.close_callback = nullptr;
            break;
        }
        if (!we.window) {
            empty_windows_number++;
        }
    }
    if (empty_windows_number >= 8) {
        compact_window_register();
    }
    g_register_mutex.unlock();
}

void window_sdl::start(unsigned width, unsigned height, unsigned flags, window_close_callback *callback) {
    if (!g_sdl_initialized) {
        status_notifier<task_status> initialization;
        std::thread events_thread(window_sdl::event_loop, &initialization);
        events_thread.detach();
        initialization.wait_for_status(kTaskComplete);
        if (!g_sdl_initialized) {
            fprintf(stderr, "error: unable to open window, cannot initialize SDL2.\n");
            fflush(stderr);
            return;
        }
    }
    set_status(graphics::window_starting);
    if (sdl_thread_create_window("Graphics Window", width, height, flags, callback)) {
        wait_for_status(graphics::window_running);
    }
}

void window_sdl::update_region(const graphics::image& src_img, const agg::rect_i& r) {
    SDL_Rect rect;
    rect.x = r.x1;
    rect.y = src_img.height() - r.y2;
    rect.w = r.x2 - r.x1;
    rect.h = r.y2 - r.y1;
    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    Uint8 *pixels;
    int pitch;
    SDL_LockTexture(m_texture, &rect, (void **) &pixels, &pitch);
    rendering_buffer dst(pixels, r.x2 - r.x1, r.y2 - r.y1, -pitch);
    dst.copy_from(src_view);
    SDL_UnlockTexture(m_texture);
    SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}

/* When querying about the status of the window in the method below and
   in send_close_window_event we should use a mutex to ensure that the state of
   the window doesn't change between the query and the SDL_PushEvent.
   If this case the mutex should be appropriately locked/unlocked in the event loop.
   We do not use the mutex for staty simple and because SDL_PushEvent is
   thread safe and we consider pushing an event for a window not yet started or
   closed as a benign error. */
bool window_sdl::send_update_region_event() {
    SDL_Event event;
    SDL_zero(event);
    event.type = window_sdl::g_user_event_type;
    event.user.code = kUpdateRegion;
    event.user.data1 = (void *) (intptr_t) SDL_GetWindowID(m_window);
    if (status() == graphics::window_running) {
        return (SDL_PushEvent(&event) >= 0);
    }
    return false;
}

bool window_sdl::sdl_thread_create_window(const char *caption,
    unsigned width, unsigned height, unsigned flags, window_close_callback *callback)
{
    // This function post an event to the main SDL thread requesting
    // a window creation. We attach some data to the event
    // with the instance address (this), the "create" object to notify about
    // the window's creation and the "close callback".
    complete_notify<window_create_message> create;
    create.start(window_create_message{caption, width, height, flags, window_create_message::success, this, callback});

    SDL_Event event;
    SDL_zero(event);
    event.type = window_sdl::g_user_event_type;
    event.user.code = kCreateWindow;
    event.user.data1 = (void *) &create;

    if (SDL_PushEvent(&event) >= 0) {
        create.wait();
    }
    if (create.message().return_code == window_create_message::window_error) {
        fprintf(stderr, "fatal error: cannot create SDL window\n");
        return false;
    }
    return true;
}

bool window_sdl::send_close_window_event() {
    SDL_Event event;
    SDL_zero(event);
    event.type = SDL_WINDOWEVENT;
    event.window.event = SDL_WINDOWEVENT_CLOSE;
    event.window.windowID = SDL_GetWindowID(m_window);
    auto current_status = status();
    if (current_status == graphics::window_running || current_status == graphics::window_starting) {
        return (SDL_PushEvent(&event) >= 0);
    }
    return false;
}

void window_sdl::send_quit_event() {
    SDL_Event event;
    SDL_zero(event);
    event.type = window_sdl::g_user_event_type;
    event.user.code = kQuitEventLoop;
    SDL_PushEvent(&event);
}

bool window_sdl::initialization_success() {
    return (g_sdl_init_status == 0);
}

bool window_sdl::send_request(graphics::window_request request_type, int index) {
    switch (request_type) {
    case graphics::window_request::update:
        m_update_notify.start(index);
        if (send_update_region_event()) {
            // Wait for the notification but only if the message was actually sent.
            m_update_notify.wait();
            return true;
        }
        break;
    case graphics::window_request::close:
        if (send_close_window_event()) {
            wait_for_status(graphics::window_closed);
            return true;
        }
    }
    return false;
}
