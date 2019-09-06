#ifndef LIBCANVAS_C_H_
#define LIBCANVAS_C_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    canvas_window_resize            = 1,
    canvas_window_hw_buffer         = 2,
    canvas_window_keep_aspect_ratio = 4,
    canvas_window_process_all_keys  = 8,
};

enum { canvas_color_red = 0xB40000FF, canvas_color_green = 0x00CC00ff, canvas_color_blue = 0x0000B4FF, canvas_color_yellow = 0xF5FE00FF, canvas_color_black = 0x000000FF, canvas_color_gray = 0xBBBBBBFF, canvas_color_white = 0xFFFFFFFF, canvas_color_none = 0 };

enum {
    canvas_fill    = 1 << 0,
    canvas_stroke  = 1 << 1,
    canvas_outline = 1 << 2,
    canvas_crisp   = 1 << 3,
};

enum {
    canvas_plot_show_units = 1 << 0,
    canvas_plot_auto_limits = 1 << 1
};

enum {
    canvas_plot_right  = 0,
    canvas_plot_left   = 1, 
    canvas_plot_bottom = 2,
    canvas_plot_top    = 3
};

typedef uint32_t canvas_color;

struct canvas_rectangle {
    float x1, y1, x2, y2;
};

struct canvas_object;
struct canvas_path;
struct canvas_plot;
struct canvas_window;
typedef struct canvas_object canvas_object;
typedef struct canvas_path canvas_path;
typedef struct canvas_plot canvas_plot;
typedef struct canvas_window canvas_window;

canvas_path *canvas_path_new();
canvas_path *canvas_path_copy(const canvas_path *source);
void canvas_path_free(canvas_path *path);

void canvas_path_move_to(canvas_path *path, double x, double y);
void canvas_path_line_to(canvas_path *path, double x, double y);
void canvas_path_close_polygon(canvas_path *path);

#define canvas_object(obj) ((canvas_object *) (obj))

canvas_plot *canvas_plot_new(unsigned int flags);
void canvas_plot_set_limits(canvas_plot *plot_object, float x0, float y0, float x1, float y1);
void canvas_plot_commit_pending_draw(canvas_plot *plot_object);
void canvas_plot_add(canvas_plot *plot, canvas_object *obj, canvas_color stroke_color, float stroke_width, canvas_color fill_color, int flags);
void canvas_plot_free(canvas_plot *plot);

canvas_window *canvas_window_new();
canvas_window *canvas_window_new_with_layout(const char *fmt);

int canvas_window_attach(canvas_window *win, canvas_plot *plot, const char* slot_str);
void canvas_window_slot_refresh(canvas_window *win, unsigned index);
void canvas_window_start(canvas_window *win, unsigned width, unsigned height, unsigned flags);
void canvas_window_wait(canvas_window *win);

void canvas_window_free(canvas_window *w);

void canvas_initialize_fonts();

#ifdef __cplusplus
}
#endif
#endif
