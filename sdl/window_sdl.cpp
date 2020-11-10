#include "sdl/window_sdl.h"

bool window_sdl::g_sdl_initialized = false;
Uint32 window_sdl::g_update_event_type = -1;

window_sdl::window_sdl(graphics::window_surface& window_surface):
    m_window(nullptr), m_pixel_format(agg::pix_format_undefined),
    m_window_surface(window_surface)
{
}

static agg::pix_format_e find_pixel_format(SDL_Surface *surface) {
    fprintf(stderr, "SDL pixel format: %d %s\n", surface->format->format, SDL_GetPixelFormatName(surface->format->format)); fflush(stderr);
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

void window_sdl::start_blocking(unsigned width, unsigned height, unsigned flags) {
    if (!g_sdl_initialized) {
        fprintf(stderr, "SDL initialization\n"); fflush(stderr);
        SDL_Init(SDL_INIT_VIDEO);
        fprintf(stderr, "SDL Register Event\n"); fflush(stderr);
        g_update_event_type = SDL_RegisterEvents(1);
        if (g_update_event_type == ((Uint32)-1)) {
            return;
        }
        fprintf(stderr, "SDL Register Event done: %d\n", g_update_event_type); fflush(stderr);
        g_sdl_initialized = true;
    }
    set_status(graphics::window_starting);
    // Note: we may use the flag SDL_WINDOW_ALLOW_HIGHDPI.
    m_window = SDL_CreateWindow(
        "Graphics Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        flags & graphics::window_resize ? SDL_WINDOW_RESIZABLE : 0);

    fprintf(stderr, "SDL Window Creation done\n"); fflush(stderr);

    SDL_Event event;
    bool quit = false;
    fprintf(stderr, "SDL Entering event loop\n"); fflush(stderr);
    while (!quit) {
        int event_status = SDL_WaitEvent(&event);
        if (event_status == 0) {
            break;
        }
        fprintf(stderr, "SDL event: %d\n", event.type); fflush(stderr);
        switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
                fprintf(stderr, "SDL window event SHOWN: %d\n", event.window.event); fflush(stderr);
                SDL_Surface *window_surface = SDL_GetWindowSurface(m_window);
                fprintf(stderr, "width: %d height: %d\n", window_surface->w, window_surface->h); fflush(stderr);
                m_pixel_format = find_pixel_format(window_surface);
                m_window_surface.resize(window_surface->w, window_surface->h);
                m_window_surface.render();
                set_status(graphics::window_running);
            } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                fprintf(stderr, "SDL window event RESIZED: %d\n", event.window.event); fflush(stderr);
                const int width = event.window.data1, height = event.window.data2;
                m_window_surface.resize(width, height);
                m_window_surface.render();
            } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
                fprintf(stderr, "SDL window event EXPOSED: %d\n", event.window.event); fflush(stderr);
                m_window_surface.update_window_area();
            } else if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                fprintf(stderr, "SDL window event CLOSE: %d\n", event.window.event); fflush(stderr);
                quit = true;
            } else {
                fprintf(stderr, "SDL window event UNKNOWN: %d\n", event.window.event); fflush(stderr);
            }
            break;
        default:
            if (event.type == g_update_event_type) {
                fprintf(stderr, "SDL UPDATE REGION EVENT\n"); fflush(stderr);
                if (!m_update_notify.completed) {
                    m_window_surface.slot_refresh(m_update_notify.plot_index);
                    m_update_notify.notify();
                }
            }
        }
    }
    set_status(graphics::window_closed);
}

void window_sdl::update_region(const graphics::image& src_img, const agg::rect_i& r) {
    // We may consider using the function SDL_CreateRGBSurfaceWithFormatFrom to wrap
    // the pixel data from the image and use SDL_BlitSurface to blit the pixels.
    // Unfortunately the convention for the y is opposite and AFAIK it cannot work. 
    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    SDL_Surface *window_surface = SDL_GetWindowSurface(m_window);
    Uint8 *pixels = (Uint8 *) window_surface->pixels;

    fprintf(stderr, "rect: (%d,%d) (%d,%d)\n", r.x1, r.y1, r.x2, r.y2);
    fprintf(stderr, "window surface pixels: %p bpp: %d\n", pixels, window_surface->format->BytesPerPixel); fflush(stderr);

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

bool window_sdl::send_update_region_event() {
    if (status() == graphics::window_running) {
        SDL_Event event;
        SDL_zero(event);
        event.type = window_sdl::g_update_event_type;
        SDL_PushEvent(&event);
        return true;
    }
    return false;
}

bool window_sdl::send_close_window_event() {
    auto current_status = status();
    if (current_status == graphics::window_running || current_status == graphics::window_starting) {
        SDL_Event event;
        SDL_zero(event);
        event.type = SDL_QUIT;
        SDL_PushEvent(&event);
        return true;
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
