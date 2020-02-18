// public declarations header.
#include "libelplot_c.h"

// the following are private headers.
#include "libelplot_c_forward.h"
#include "canvas_svg.h"
#include "canvas_object.h"
#include "canvas_path.h"
#include "canvas_curve.h"
#include "canvas_dashed.h"
#include "canvas_markers.h"
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "markers.h"

static agg::rgba8 ColorToRgba8(const elem_color& c) {
    return agg::rgba8((c >> 24) & 0xff, (c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
}

static void plot_update_windows_and_commit(elem_plot *plot_object) {
    graphics::plot_agent *agent = plot_object->plot_agent;
    agent->update_windows();
    elem_plot_commit_pending_draw(plot_object);
    agent->clear_pending_flags();
}

elem_object *elem_object_copy(const elem_object *obj) {
    return obj->copy();
}

void elem_object_free(elem_object *obj) {
    delete obj;
}

elem_path *elem_path_new() {
    return new elem_path{};
}

elem_path *elem_path_copy(const elem_path *path) {
    return (elem_path *) path->copy();
}

void elem_path_free(elem_path *path) {
    delete path;
}

void elem_path_move_to(elem_path *path, double x, double y) {
    path->move_to(x, y);
}

void elem_path_line_to(elem_path *path, double x, double y) {
    path->line_to(x, y);
}

void elem_path_close_polygon(elem_path *path) {
    path->close_polygon();
}

elem_curve *elem_curve_new() {
    return new elem_curve{};
}

elem_curve *elem_curve_copy(const elem_curve *source) {
    return (elem_curve *) source->copy();
}

void elem_curve_free(elem_curve *curve) {
    delete curve;
}

void elem_curve_move_to(elem_curve *curve, double x, double y) {
    curve->move_to(x, y);
}

void elem_curve_line_to(elem_curve *curve, double x, double y) {
    curve->line_to(x, y);
}

void elem_curve_curve3(elem_curve *curve, double x_ctrl, double y_ctrl, double x_to, double y_to) {
    curve->curve3(x_ctrl, y_ctrl, x_to, y_to);
}

void elem_curve_curve4(elem_curve *curve, double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to) {
    curve->curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to);
}

void elem_curve_arc_to(elem_curve *curve, double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y) {
    curve->arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
}

void elem_curve_close_polygon(elem_curve *curve) {
    curve->close_polygon();
}

elem_dashed *elem_dashed_new(double dash_len, double gap_len) {
    elem_dashed *dashed = new elem_dashed{};
    dashed->add_dash(dash_len, gap_len);
    return dashed;
}

elem_dashed *elem_dashed_copy(const elem_dashed *path) {
    return (elem_dashed *) path->copy();
}

void elem_dashed_free(elem_dashed *path) {
    delete path;
}

void elem_dashed_add_dash(elem_dashed *path, double dash_len, double gap_len) {
    path->add_dash(dash_len, gap_len);
}

elem_object *elem_marker_symbol_by_index(int index) {
    return new_marker_symbol(index);
}

elem_object *elem_marker_symbol_by_name(const char *name) {
    return new_marker_symbol(name);
}

elem_markers *elem_markers_new(double size, elem_object *marker_symbol) {
    return new elem_markers(size, marker_symbol);
}

elem_markers *elem_markers_copy(const elem_markers *markers) {
    return (elem_markers *) markers->copy();
}

void elem_markers_free(elem_markers *markers) {
    delete markers;
}

elem_plot *elem_plot_new(unsigned int flags) {
    elem_plot *plot_struct = new elem_plot();
    if (plot_struct) {
        plot_struct->plot = new graphics::plot{flags};
        plot_struct->plot_agent = new graphics::plot_agent{};
    }
    return plot_struct;
}

void elem_plot_free(elem_plot *plot_object) {
    delete plot_object->plot;
    delete plot_object->plot_agent;
    delete plot_object;
}

void elem_plot_set_title(elem_plot *plot_object, const char *title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_title(title);
    }
    plot_update_windows_and_commit(plot_object);
}

void elem_plot_set_x_axis_title(elem_plot *plot_object, const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_x_axis_title(axis_title);
    }
    plot_update_windows_and_commit(plot_object);
}

void elem_plot_set_y_axis_title(elem_plot *plot_object, const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_y_axis_title(axis_title);
    }
    plot_update_windows_and_commit(plot_object);
}

void elem_plot_set_label_angle(elem_plot *plot_object, int axis, float angle) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_axis_labels_angle(axis == elem_x_axis ? graphics::x_axis : graphics::y_axis, angle);
    }
    plot_update_windows_and_commit(plot_object);
}

void elem_plot_enable_label_format(elem_plot *plot_object, int axis, const char *fmt) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->enable_label_format(axis == elem_x_axis ? graphics::x_axis : graphics::y_axis, fmt);
    }
    plot_update_windows_and_commit(plot_object);
}

void elem_plot_set_clip_mode(elem_plot *plot_object, bool flag) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    graphics::plot::drawing_context dc(*p);
    p->set_clip_mode(flag);
}

bool elem_plot_push_layer(elem_plot *plot_object) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    bool success = false;
    {
        graphics::plot::drawing_context dc(*p);
        success = p->push_layer();
    }
    if (success) {
        plot_update_windows_and_commit(plot_object);
    }
    return success;
}

bool elem_plot_pop_layer(elem_plot *plot_object) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    bool success = false;
    {
        graphics::plot::drawing_context dc(*p);
        success = p->pop_layer();
    }
    if (success) {
        plot_update_windows_and_commit(plot_object);
    }
    return success;
}

void elem_plot_clear_layer(elem_plot *plot_object) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->clear_current_layer();
    }
    plot_update_windows_and_commit(plot_object);
}

void elem_plot_set_limits(elem_plot *plot_object, float x1, float y1, float x2, float y2) {
    graphics::plot *p = plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_limits(agg::rect_d(x1, y1, x2, y2));
    }
    plot_update_windows_and_commit(plot_object);
}

void elem_plot_commit_pending_draw(elem_plot *plot_object) {
    graphics::plot *p = plot_object->plot;
    graphics::plot::drawing_context dc(*p);
    p->commit_pending_draw();
}

// The plot takes implicitly the ownership of the object.
void elem_plot_add(elem_plot *plot_object, elem_object *obj, elem_color stroke_color, float stroke_width, elem_color fill_color, int flags) {
    graphics::plot *p = plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->add(obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
    }
    plot_update_windows_and_commit(plot_object);
}

int elem_plot_write_svg(elem_plot *plot_object, const char *filename, double width, double height) {
    FILE *svg_file = fopen(filename, "w");
    if (!svg_file) {
        return false;
    }
    canvas_svg canvas{svg_file, height};
    agg::trans_affine_scaling m(width, height);
    canvas.write_header(width, height);
    graphics::plot *p = plot_object->plot;
    graphics::plot::drawing_context dc(*p);
    p->draw(dc, canvas, m, nullptr);
    canvas.write_end();
    fclose(svg_file);
    return true;
}

void elem_initialize_fonts() {
    graphics::initialize_fonts();
}
