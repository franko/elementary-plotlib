#include "sdl/window_sdl.h"

bool window_sdl::g_sdl_initialized = false;
Uint32 window_sdl::g_update_event_type = -1;

window_sdl::window_sdl(graphics::window_surface& window_surface):
    m_width(0), m_height(0), m_window(nullptr), m_pixel_format(agg::pix_format_undefined),
    m_window_surface(window_surface)
{
}

window_sdl::~window_sdl() {
}

static agg::pix_format_e find_pixel_format(SDL_Surface *surface) {
    fprintf(stderr, "SDL pixel format: %d %s\n", surface->format->format, SDL_GetPixelFormatName(surface->format->format)); fflush(stderr);
    switch (surface->format->format) {
        // FIXME: add other cases of interest.
        // consider add all cases treated in rendering_buffer_utils.h, function rendering_buffer_copy.
        case SDL_PIXELFORMAT_BGR24:
        case SDL_PIXELFORMAT_BGR888:
            return agg::pix_format_bgra32;
        case SDL_PIXELFORMAT_RGB24:
        case SDL_PIXELFORMAT_RGB888:
            // FIXME: bgra32 seems to work below but I do not understand why.
            return agg::pix_format_bgra32;
        case SDL_PIXELFORMAT_BGRA32:
        case SDL_PIXELFORMAT_BGRA8888:
            return agg::pix_format_bgra32;
        case SDL_PIXELFORMAT_RGBA32:
        case SDL_PIXELFORMAT_RGBA8888:
            return agg::pix_format_rgba32;
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
    // FIXME: manage flags for resizable window.
    m_window = SDL_CreateWindow(
        "Graphics Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_RESIZABLE); // | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);

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
                // m_window_surface.update_window_area();
                fprintf(stderr, "SDL window event SHOWN: %d\n", event.window.event); fflush(stderr);
                SDL_Surface *window_surface = SDL_GetWindowSurface(m_window);
                // TODO: check if we need to store m_width and m_height.
                fprintf(stderr, "width: %d height: %d\n", window_surface->w, window_surface->h); fflush(stderr);
                if (window_surface->w != m_width || window_surface->h != m_height) {
                    m_pixel_format = find_pixel_format(window_surface);
                    m_window_surface.resize(window_surface->w, window_surface->h);
                    m_width = window_surface->w;
                    m_height = window_surface->h;
                    m_window_surface.render();
                    m_window_surface.update_window_area();
                    // SDL_UpdateWindowSurface(m_window);
                }
                set_status(graphics::window_running);
            } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                fprintf(stderr, "SDL window event RESIZED: %d\n", event.window.event); fflush(stderr);
                int width = event.window.data1, height = event.window.data2;
                // TODO: check if we need to store m_width and m_height.
                if (width != m_width || height != m_height) {
                    m_window_surface.resize(width, height);
                    m_width = width;
                    m_height = height;
                    m_window_surface.render();
                }
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

    // FIXME: use SDL_UpdateWindowSurfaceRects instead.
    SDL_UpdateWindowSurface(m_window);
}

bool window_sdl::send_update_region_event() {
    SDL_Event event;
    SDL_zero(event);
    event.type = window_sdl::g_update_event_type;
    SDL_PushEvent(&event);
    return true;
}

bool window_sdl::send_close_window_event() {
    SDL_Event event;
    SDL_zero(event);
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    return true;
}

bool window_sdl::send_request(graphics::window_request request_type, int index) {
    // lock();
    switch (request_type) {
    case graphics::window_request::update:
        m_update_notify.start(index);
        if (send_update_region_event()) {
            // Wait for the notification but only if the message was actually sent.
            // unlock();
            m_update_notify.wait();
            return true;
        }
        break;
    case graphics::window_request::close:
        if (send_close_window_event()) {
            // unlock();
            // wait_for_status(graphics::window_closed);
            return true;
        }
    }
    // unlock();
    return false;
}
