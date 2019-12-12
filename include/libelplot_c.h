/*
** Libelplot -- a plotting library for C++. https://github.com/franko/libcanvas
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

#ifndef LIBELPLOT_C_H_
#define LIBELPLOT_C_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    elp_window_resize            = 1,
    elp_window_hw_buffer         = 2,
    elp_window_keep_aspect_ratio = 4,
    elp_window_process_all_keys  = 8,
};

enum { elp_color_red = 0xB40000FF, elp_color_green = 0x00CC00ff, elp_color_blue = 0x0000B4FF, elp_color_yellow = 0xF5FE00FF, elp_color_black = 0x000000FF, elp_color_gray = 0xBBBBBBFF, elp_color_white = 0xFFFFFFFF, elp_color_none = 0 };

enum {
    elp_fill    = 1 << 0,
    elp_stroke  = 1 << 1,
    elp_outline = 1 << 2,
    elp_crisp   = 1 << 3,
};

enum {
    elp_plot_show_units = 1 << 0,
    elp_plot_auto_limits = 1 << 1
};

enum {
    elp_plot_right  = 0,
    elp_plot_left   = 1, 
    elp_plot_bottom = 2,
    elp_plot_top    = 3
};

enum {
    elp_x_axis = 0,
    elp_y_axis = 1,
};

typedef uint32_t elp_color;

struct elp_rectangle {
    float x1, y1, x2, y2;
};

#ifdef __cplusplus
class elp_object;
class elp_path;
class elp_curve;
class elp_dashed;
class elp_markers;
class elp_plot;
class elp_window;
#else
struct elp_object;
struct elp_path;
struct elp_curve;
struct elp_dashed;
struct elp_markers;
struct elp_plot;
struct elp_window;
typedef struct elp_object elp_object;
typedef struct elp_path elp_path;
typedef struct elp_curve elp_curve;
typedef struct elp_dashed elp_dashed;
typedef struct elp_markers elp_markers;
typedef struct elp_plot elp_plot;
typedef struct elp_window elp_window;
#endif

elp_object *elp_object_copy(const elp_object *obj);
void elp_object_free(elp_object *obj);

elp_path *elp_path_new();
elp_path *elp_path_copy(const elp_path *source);
void elp_path_free(elp_path *path);

void elp_path_move_to(elp_path *path, double x, double y);
void elp_path_line_to(elp_path *path, double x, double y);
void elp_path_close_polygon(elp_path *path);

elp_curve *elp_curve_new();
elp_curve *elp_curve_copy(const elp_curve *source);
void elp_curve_free(elp_curve *curve);
void elp_curve_move_to(elp_curve *curve, double x, double y);
void elp_curve_line_to(elp_curve *curve, double x, double y);
void elp_curve_curve3(elp_curve *curve, double x_ctrl, double y_ctrl, double x_to, double y_to);
void elp_curve_curve4(elp_curve *curve, double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to);
void elp_curve_arc_to(elp_curve *curve, double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y);
void elp_curve_close_polygon(elp_curve *curve);

elp_dashed *elp_dashed_new(double dash_len, double gap_len);
void elp_dashed_add_dash(elp_dashed *path, double dash_len, double gap_len);
void elp_dashed_free(elp_dashed *path);
elp_dashed *elp_dashed_copy(const elp_dashed *path);

elp_object *elp_marker_symbol_by_index(int index);
elp_object *elp_marker_symbol_by_name(const char *name);
elp_markers *elp_markers_new(double size, elp_object *marker_symbol);
elp_markers *elp_markers_copy(const elp_markers *markers);
void elp_markers_free(elp_markers *markers);

elp_plot *elp_plot_new(unsigned int flags);
// TODO: implement functions to copy a plot
void elp_plot_free(elp_plot *plot);
void elp_plot_set_title(elp_plot *plot, const char *title);
void elp_plot_set_x_axis_title(elp_plot *plot, const char *title);
void elp_plot_set_y_axis_title(elp_plot *plot, const char *title);
void elp_plot_set_label_angle(elp_plot *plot, int axis, float angle);
void elp_plot_set_limits(elp_plot *plot, float x0, float y0, float x1, float y1);
void elp_plot_set_clip_mode(elp_plot *plot, bool flag);
void elp_plot_enable_label_format(elp_plot *plot, int axis, const char *fmt);
void elp_plot_commit_pending_draw(elp_plot *plot_object);
bool elp_plot_push_layer(elp_plot *plot);
bool elp_plot_pop_layer(elp_plot *plot);
void elp_plot_clear_layer(elp_plot *plot);
void elp_plot_add(elp_plot *plot, elp_object *obj, elp_color stroke_color, float stroke_width, elp_color fill_color, int flags);
// TODO: add function to add a legend
int elp_plot_write_svg(elp_plot *plot, const char *filename, double width, double height);

elp_window *elp_window_new();
elp_window *elp_window_new_with_layout(const char *fmt);
void elp_window_set_layout(elp_window *win, const char *fmt);
void elp_window_free(elp_window *w);
int elp_window_attach(elp_window *win, elp_plot *plot, const char* slot_str);
void elp_window_slot_refresh(elp_window *win, unsigned index);
void elp_window_start(elp_window *win, unsigned width, unsigned height, unsigned flags);
void elp_window_wait(elp_window *win);

// TODO: document initialize fonts function
void elp_initialize_fonts();

// TODO: document all the cast functions below
static inline elp_object *path_as_object(elp_path *p) {
    return (elp_object *) p;
}

static inline elp_object *curve_as_object(elp_curve *c) {
    return (elp_object *) c;
}

static inline elp_object *dashed_as_object(elp_dashed *c) {
    return (elp_object *) c;
}

static inline elp_path *dashed_as_path(elp_dashed *c) {
    return (elp_path *) c;
}

static inline elp_path *markers_as_path(elp_markers *c) {
    return (elp_path *) c;
}

static inline elp_object *markers_as_object(elp_markers *c) {
    return (elp_object *) c;
}

#ifdef __cplusplus
}
#endif
#endif
