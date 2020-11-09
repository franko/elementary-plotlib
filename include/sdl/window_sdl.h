#include <SDL.h>

#include "display_window_status.h"
#include "start_window.h"
#include "strpp.h"
#include "update_region_notify.h"
#include "window_surface.h"

class window_sdl : public graphics::display_window_status {
public:
    window_sdl(graphics::window_surface& window_surface);
    ~window_sdl();

    void start_blocking(unsigned width, unsigned height, unsigned flags);

    void start(unsigned width, unsigned height, unsigned flags, window_close_callback *callback) {
        int status = start_window_new_thread(this, width, height, flags, callback);
        if (status != 0) {
            debug_log(1, "error starting window, unknwon error");
        }
    }

    bool send_request(graphics::window_request request_type, int plot_index) override;
    void update_region(const graphics::image& img, const agg::rect_i& r) override;

private:
    // bool init(unsigned width, unsigned height, unsigned flags);
    int run();
    void close();

    bool send_update_region_event();
    bool send_close_window_event();

    int m_width;
    int m_height;
    SDL_Window *m_window;
    agg::pix_format_e m_pixel_format;
    str m_caption;
    update_region_notify m_update_notify;
    graphics::window_surface& m_window_surface;

    static Uint32 g_update_event_type;
    static bool g_sdl_initialized;
};

