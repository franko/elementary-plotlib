#include <mutex>
#include <thread>

#include <SDL.h>
#include <agg_array.h>

#include "display_window_status.h"
#include "status_notifier.h"
#include "strpp.h"
#include "update_region_notify.h"
#include "window_close_callback.h"
#include "window_surface.h"

class window_sdl;

struct window_entry {
    window_sdl *window;
    Uint32 window_id;
    window_close_callback *close_callback;
};

class window_sdl : public graphics::display_window_status {
    enum { kUpdateRegion, kCreateWindow };
public:
    window_sdl(graphics::window_surface& window_surface);

    void start(unsigned width, unsigned height, unsigned flags, window_close_callback *callback);
    bool send_request(graphics::window_request request_type, int plot_index) override;
    void update_region(const graphics::image& img, const agg::rect_i& r) override;

    static void event_loop(status_notifier<task_status> *initialization);
    static void send_quit_event();
    static bool initialization_success();

private:
    int run();
    void close();

    bool send_update_region_event();
    bool send_close_window_event();
    bool sdl_thread_create_window(const char *caption, unsigned w, unsigned h, unsigned flags, window_close_callback *callback);

    void register_window(SDL_Window *window, window_close_callback *close_callback);
    void unregister_window();
    void process_window_event(SDL_Event *event);
    void process_update_event();

    void setup_sdl_window(SDL_Window *window);
    void setup_renderer(int w, int h);
    static int initialize_sdl();
    static window_sdl *select_on_window_id(Uint32 window_id);
    static void compact_window_register();

    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_Texture *m_texture;
    update_region_notify m_update_notify;
    graphics::window_surface& m_window_surface;

    // We keep a global register of all the windows to dispatch events.
    // The mutex is used to protect access to the register.
    static std::mutex g_register_mutex;
    static agg::pod_bvector<window_entry> g_window_entries;

    static Uint32 g_user_event_type;
    static bool g_sdl_initialized;
    static int g_sdl_init_status;
};
