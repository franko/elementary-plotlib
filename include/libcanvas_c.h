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
void canvas_path_close_polygon(canvas_path *path, double x, double y);

#define canvas_object(obj) ((canvas_object *) (obj))

canvas_plot *canvas_plot_new(int flags);
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

#ifdef __cplusplus
}
#endif
#endif
