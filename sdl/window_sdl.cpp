#include "window_sdl.h"

bool window_sdl::g_sdl_initialized = false;
Uint32 window_sdl::g_update_event_type = -1;

window_sdl::window_sdl(graphics::window_surface& window_surface):
    m_width(0), m_height(0), m_window(nullptr) {
}

window_sdl::~window_sdl() {
}

void window_sdl::start_blocking(unsigned width, unsigned height, unsigned flags) {
    if (!g_sdl_initialized) {
        SDL_Init(SDL_INIT_VIDEO);
        g_update_event_type = SDL_RegisterEvents(1);
        if (g_update_event_type == ((Uint32)-1)) {
            return;
        }
        g_sdl_initialized = true;
    }
    // FIXME: manage flags for resizable window.
    m_window = SDL_CreateWindow(
        "Graphics Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_RESIZABLE); // | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);

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
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                unsigned width = event.window.data1, height = event.window.data2;
                // TODO: check if we need to store m_width and m_height.
                if (width != m_width || height != m_height) {
                    m_window_surface.resize(width, height);
                    m_width = width;
                    m_height = height;
                    m_window_surface.render();
                }
            } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
                m_window_surface.update_window_area();
            }
            break;
        case g_update_event_type:
            if (!m_update_notify.completed) {
                m_window_surface.slot_refresh(m_update_notify.plot_index);
                m_update_notify.notify();
            }
            break;
        }
    }
}
