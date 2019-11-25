/*
** Libcanvas -- a plotting library for C++. https://github.com/franko/libcanvas
**
** Copyright (C) 2018-2019 Francesco Abbate. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#ifndef LIBCANVAS_C_H_
#define LIBCANVAS_C_H_

#include <stdint.h>
#include <stdbool.h>

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

enum {
    canvas_x_axis = 0,
    canvas_y_axis = 1,
};

typedef uint32_t canvas_color;

struct canvas_rectangle {
    float x1, y1, x2, y2;
};

#ifdef __cplusplus
class canvas_object;
class canvas_path;
class canvas_curve;
class canvas_dashed;
class canvas_markers;
class canvas_plot;
class canvas_window;
#else
struct canvas_object;
struct canvas_path;
struct canvas_curve;
struct canvas_dashed;
struct canvas_markers;
struct canvas_plot;
struct canvas_window;
typedef struct canvas_object canvas_object;
typedef struct canvas_path canvas_path;
typedef struct canvas_curve canvas_curve;
typedef struct canvas_dashed canvas_dashed;
typedef struct canvas_markers canvas_markers;
typedef struct canvas_plot canvas_plot;
typedef struct canvas_window canvas_window;
#endif

canvas_object *canvas_object_copy(const canvas_object *obj);
void canvas_object_free(canvas_object *obj);

canvas_path *canvas_path_new();
canvas_path *canvas_path_copy(const canvas_path *source);
void canvas_path_free(canvas_path *path);

void canvas_path_move_to(canvas_path *path, double x, double y);
void canvas_path_line_to(canvas_path *path, double x, double y);
void canvas_path_close_polygon(canvas_path *path);

canvas_curve *canvas_curve_new();
canvas_curve *canvas_curve_copy(const canvas_curve *source);
void canvas_curve_free(canvas_curve *curve);
void canvas_curve_move_to(canvas_curve *curve, double x, double y);
void canvas_curve_line_to(canvas_curve *curve, double x, double y);
void canvas_curve_curve3(canvas_curve *curve, double x_ctrl, double y_ctrl, double x_to, double y_to);
void canvas_curve_curve4(canvas_curve *curve, double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to);
void canvas_curve_arc_to(canvas_curve *curve, double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y);
void canvas_curve_close_polygon(canvas_curve *curve);

canvas_dashed *canvas_dashed_new(double dash_len, double gap_len);
void canvas_dashed_add_dash(canvas_dashed *path, double dash_len, double gap_len);
void canvas_dashed_free(canvas_dashed *path);
canvas_dashed *canvas_dashed_copy(const canvas_dashed *path);

canvas_object *canvas_marker_symbol_by_index(int index);
canvas_object *canvas_marker_symbol_by_name(const char *name);
canvas_markers *canvas_markers_new(double size, canvas_object *marker_symbol);
canvas_markers *canvas_markers_copy(const canvas_markers *markers);
void canvas_markers_free(canvas_markers *markers);

canvas_plot *canvas_plot_new(unsigned int flags);
// TODO: implement functions to copy a plot
void canvas_plot_free(canvas_plot *plot);
void canvas_plot_set_title(canvas_plot *plot, const char *title);
void canvas_plot_set_x_axis_title(canvas_plot *plot, const char *title);
void canvas_plot_set_y_axis_title(canvas_plot *plot, const char *title);
void canvas_plot_set_label_angle(canvas_plot *plot, int axis, float angle);
void canvas_plot_set_limits(canvas_plot *plot, float x0, float y0, float x1, float y1);
void canvas_plot_set_clip_mode(canvas_plot *plot, bool flag);
void canvas_plot_enable_label_format(canvas_plot *plot, int axis, const char *fmt);
void canvas_plot_commit_pending_draw(canvas_plot *plot_object);
bool canvas_plot_push_layer(canvas_plot *plot);
bool canvas_plot_pop_layer(canvas_plot *plot);
void canvas_plot_clear_layer(canvas_plot *plot);
void canvas_plot_add(canvas_plot *plot, canvas_object *obj, canvas_color stroke_color, float stroke_width, canvas_color fill_color, int flags);
// TODO: add function to add a legend
int canvas_plot_write_svg(canvas_plot *plot, const char *filename, double width, double height);

canvas_window *canvas_window_new();
canvas_window *canvas_window_new_with_layout(const char *fmt);
void canvas_window_set_layout(canvas_window *win, const char *fmt);
void canvas_window_free(canvas_window *w);
int canvas_window_attach(canvas_window *win, canvas_plot *plot, const char* slot_str);
void canvas_window_slot_refresh(canvas_window *win, unsigned index);
void canvas_window_start(canvas_window *win, unsigned width, unsigned height, unsigned flags);
void canvas_window_wait(canvas_window *win);

// TODO: document initialize fonts function
void canvas_initialize_fonts();

// TODO: document all the cast functions below
static inline canvas_object *path_as_object(canvas_path *p) {
    return (canvas_object *) p;
}

static inline canvas_object *curve_as_object(canvas_curve *c) {
    return (canvas_object *) c;
}

static inline canvas_object *dashed_as_object(canvas_dashed *c) {
    return (canvas_object *) c;
}

static inline canvas_path *dashed_as_path(canvas_dashed *c) {
    return (canvas_path *) c;
}

static inline canvas_path *markers_as_path(canvas_markers *c) {
    return (canvas_path *) c;
}

static inline canvas_object *markers_as_object(canvas_markers *c) {
    return (canvas_object *) c;
}

#ifdef __cplusplus
}
#endif
#endif
