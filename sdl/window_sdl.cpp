#ifdef WIN32
  #include <windows.h>
#endif
#include <stdint.h>

#include "sdl/window_sdl.h"

struct create_window_data {
    window_sdl *this_window;
    window_create_notify *create;
    window_close_callback *callback;
};

bool window_sdl::g_sdl_initialized = false;
Uint32 window_sdl::g_user_event_type = -1;
std::mutex window_sdl::g_register_mutex;
agg::pod_bvector<window_entry> window_sdl::g_window_entries;

window_sdl::window_sdl(graphics::window_surface& window_surface):
    m_window(nullptr), m_pixel_format(agg::pix_format_undefined),
    m_window_surface(window_surface)
{
}

static agg::pix_format_e find_pixel_format(SDL_Surface *surface) {
    switch (surface->format->format) {
        case SDL_PIXELFORMAT_ABGR8888: // equal to SDL_PIXELFORMAT_RGBA32
        case SDL_PIXELFORMAT_BGR888:
            return agg::pix_format_rgba32;
        case SDL_PIXELFORMAT_ARGB8888: // equal to SDL_PIXELFORMAT_BGRA32
        case SDL_PIXELFORMAT_RGB888:
            return agg::pix_format_bgra32;
        case SDL_PIXELFORMAT_BGR24:
            return agg::pix_format_bgr24;
        case SDL_PIXELFORMAT_RGB24:
            return agg::pix_format_rgb24;
        default:
            break;
    }
    return agg::pix_format_undefined;
}

void window_sdl::process_window_event(SDL_Event *event) {
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN: {
        SDL_Surface *window_surface = SDL_GetWindowSurface(m_window);
        m_pixel_format = find_pixel_format(window_surface);
        m_window_surface.resize(window_surface->w, window_surface->h);
        m_window_surface.render();
        set_status(graphics::window_running);
        break;
    }
    case SDL_WINDOWEVENT_RESIZED: {
        const int width = event->window.data1, height = event->window.data2;
        m_window_surface.resize(width, height);
        m_window_surface.render();
        break;
    }
    case SDL_WINDOWEVENT_EXPOSED:
        m_window_surface.update_window_area();
        break;
    case SDL_WINDOWEVENT_CLOSE:
        SDL_DestroyWindow(m_window);
        unregister_window();
        set_status(graphics::window_closed);
        break;
    default:
        break;
    }
}

void window_sdl::process_update_event() {
    if (!m_update_notify.completed) {
        m_window_surface.slot_refresh(m_update_notify.plot_index);
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
    if (SDL_Init(SDL_INIT_VIDEO)) {
        return (-1);
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
    bool quit = false;
    while (!quit) {
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
                    create_window_data *data = (create_window_data *) event.user.data1;
                    const window_create_message& message = data->create->message;
                    Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI | (message.flags & graphics::window_resize ? SDL_WINDOW_RESIZABLE : 0);
                    SDL_Window *window = SDL_CreateWindow(message.caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, message.width, message.height, window_flags);
                    data->this_window->register_window(window, data->callback);
                    data->create->notify();
                }
            }
        }
    }
}

void window_sdl::register_window(SDL_Window *window, window_close_callback *close_callback) {
    m_window = window;
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
    send_create_window_event("Graphics Window", width, height, flags, callback);
    wait_for_status(graphics::window_running);
}

void window_sdl::update_region(const graphics::image& src_img, const agg::rect_i& r) {
    // We may consider using the function SDL_CreateRGBSurfaceWithFormatFrom to wrap
    // the pixel data from the image and use SDL_BlitSurface to blit the pixels.
    // Unfortunately the convention for the y sign is opposite and I know no way
    // to make it work with SDL blit function.
    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    SDL_Surface *window_surface = SDL_GetWindowSurface(m_window);
    Uint8 *pixels = (Uint8 *) window_surface->pixels;

    const int window_bpp = window_surface->format->BytesPerPixel;
    rendering_buffer dst(pixels, window_surface->w, window_surface->h, -window_surface->w * window_bpp);
    rendering_buffer dst_view;
    rendering_buffer_get_view(dst_view, dst, r, window_bpp);

    rendering_buffer_copy(dst_view, m_pixel_format, src_view, (agg::pix_format_e) graphics::pixel_format);

    SDL_Rect rect;
    rect.x = r.x1;
    rect.y = window_surface->h - r.y2;
    rect.w = r.x2 - r.x1;
    rect.h = r.y2 - r.y1;
    SDL_UpdateWindowSurfaceRects(m_window, &rect, 1);
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

void window_sdl::send_create_window_event(const char *caption,
    unsigned width, unsigned height, unsigned flags, window_close_callback *callback)
{
    SDL_Event event;
    SDL_zero(event);
    event.type = window_sdl::g_user_event_type;
    event.user.code = kCreateWindow;
    window_create_notify create;
    create_window_data data = {this, &create, callback};
    event.user.data1 = (void *) &data;
    create.start(caption, width, height, flags);
    if (SDL_PushEvent(&event) >= 0) {
        create.wait();
    }
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
