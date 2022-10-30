/*
** Elementary Plot -- a plotting library for C++. https://github.com/franko/elementary-plotlib
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

#ifndef ELEM_PLOT_C_H_
#define ELEM_PLOT_C_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    elem_window_resize            = 1,
    elem_window_hw_buffer         = 2,
    elem_window_keep_aspect_ratio = 4,
    elem_window_process_all_keys  = 8,
};

enum { elem_color_red = 0xB40000FF, elem_color_green = 0x00CC00ff, elem_color_blue = 0x0000B4FF, elem_color_yellow = 0xF5FE00FF, elem_color_black = 0x000000FF, elem_color_gray = 0xBBBBBBFF, elem_color_white = 0xFFFFFFFF, elem_color_none = 0 };

enum {
    elem_fill    = 1 << 0,
    elem_stroke  = 1 << 1,
    elem_outline = 1 << 2,
    elem_crisp   = 1 << 3,
};

enum {
    elem_plot_show_units = 1 << 0,
    elem_plot_auto_limits = 1 << 1,
    elem_plot_clip_region = 1 << 2
};

enum {
    elem_plot_right  = 0,
    elem_plot_left   = 1, 
    elem_plot_bottom = 2,
    elem_plot_top    = 3
};

enum {
    elem_x_axis = 0,
    elem_y_axis = 1,
};

typedef uint32_t elem_color;

struct elem_rectangle {
    float x1, y1, x2, y2;
};

#ifdef __cplusplus
class elem_object;
class elem_path;
class elem_curve;
class elem_dashed;
class elem_markers;
class elem_plot;
class elem_window;
#else
struct elem_object;
struct elem_path;
struct elem_curve;
struct elem_dashed;
struct elem_markers;
struct elem_plot;
struct elem_window;
typedef struct elem_object elem_object;
typedef struct elem_path elem_path;
typedef struct elem_curve elem_curve;
typedef struct elem_dashed elem_dashed;
typedef struct elem_markers elem_markers;
typedef struct elem_plot elem_plot;
typedef struct elem_window elem_window;
#endif

elem_object *elem_object_copy(const elem_object *obj);
void elem_object_free(elem_object *obj);

elem_path *elem_path_new();
elem_path *elem_path_copy(const elem_path *source);
void elem_path_free(elem_path *path);

void elem_path_move_to(elem_path *path, double x, double y);
void elem_path_line_to(elem_path *path, double x, double y);
void elem_path_close_polygon(elem_path *path);

elem_curve *elem_curve_new();
elem_curve *elem_curve_copy(const elem_curve *source);
void elem_curve_free(elem_curve *curve);
void elem_curve_move_to(elem_curve *curve, double x, double y);
void elem_curve_line_to(elem_curve *curve, double x, double y);
void elem_curve_curve3(elem_curve *curve, double x_ctrl, double y_ctrl, double x_to, double y_to);
void elem_curve_curve4(elem_curve *curve, double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to);
void elem_curve_arc_to(elem_curve *curve, double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y);
void elem_curve_close_polygon(elem_curve *curve);

elem_dashed *elem_dashed_new(double dash_len, double gap_len);
void elem_dashed_add_dash(elem_dashed *path, double dash_len, double gap_len);
void elem_dashed_free(elem_dashed *path);
elem_dashed *elem_dashed_copy(const elem_dashed *path);

elem_object *elem_marker_symbol_by_index(int index);
elem_object *elem_marker_symbol_by_name(const char *name);
elem_markers *elem_markers_new(double size, elem_object *marker_symbol);
elem_markers *elem_markers_copy(const elem_markers *markers);
void elem_markers_free(elem_markers *markers);

elem_plot *elem_plot_new(unsigned int flags);
// TODO: implement functions to copy a plot
void elem_plot_free(elem_plot *plot);
void elem_plot_set_title(elem_plot *plot, const char *title);
void elem_plot_set_x_axis_title(elem_plot *plot, const char *title);
void elem_plot_set_y_axis_title(elem_plot *plot, const char *title);
void elem_plot_set_label_angle(elem_plot *plot, int axis, float angle);
void elem_plot_set_limits(elem_plot *plot, float x0, float y0, float x1, float y1);
void elem_plot_set_clip_mode(elem_plot *plot, bool flag);
void elem_plot_enable_label_format(elem_plot *plot, int axis, const char *fmt);
void elem_plot_commit_pending_draw(elem_plot *plot_object);
bool elem_plot_push_layer(elem_plot *plot);
bool elem_plot_pop_layer(elem_plot *plot);
void elem_plot_clear_layer(elem_plot *plot);
void elem_plot_add(elem_plot *plot, elem_object *obj, elem_color stroke_color, float stroke_width, elem_color fill_color, int flags);
void elem_plot_show(elem_plot *plot, unsigned width, unsigned height, unsigned flags);
void elem_plot_wait(elem_plot *plot);
// TODO: add function to add a legend
int elem_plot_write_svg(elem_plot *plot, const char *filename, double width, double height);

elem_window *elem_window_new();
elem_window *elem_window_new_with_layout(const char *fmt);
void elem_window_set_layout(elem_window *win, const char *fmt);
void elem_window_free(elem_window *w);
int elem_window_attach(elem_window *win, elem_plot *plot, const char* slot_str);
void elem_window_slot_refresh(elem_window *win, unsigned index);
void elem_window_start(elem_window *win, unsigned width, unsigned height, unsigned flags);
void elem_window_wait(elem_window *win);

// TODO: document initialize fonts function
void elem_initialize_fonts();
int elem_initialize_and_run(int (*user_main)());

// TODO: document all the cast functions below
static inline elem_object *path_as_object(elem_path *p) {
    return (elem_object *) p;
}

static inline elem_object *curve_as_object(elem_curve *c) {
    return (elem_object *) c;
}

static inline elem_object *dashed_as_object(elem_dashed *c) {
    return (elem_object *) c;
}

static inline elem_path *dashed_as_path(elem_dashed *c) {
    return (elem_path *) c;
}

static inline elem_path *markers_as_path(elem_markers *c) {
    return (elem_path *) c;
}

static inline elem_object *markers_as_object(elem_markers *c) {
    return (elem_object *) c;
}

#ifdef __cplusplus
}
#endif
#endif
