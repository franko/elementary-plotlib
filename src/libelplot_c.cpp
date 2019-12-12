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

static agg::rgba8 ColorToRgba8(const elp_color& c) {
    return agg::rgba8((c >> 24) & 0xff, (c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
}

static void plot_update_windows_and_commit(elp_plot *plot_object) {
    graphics::plot_agent *agent = plot_object->plot_agent;
    agent->update_windows();
    elp_plot_commit_pending_draw(plot_object);
}

elp_object *elp_object_copy(const elp_object *obj) {
    return obj->copy();
}

void elp_object_free(elp_object *obj) {
    delete obj;
}

elp_path *elp_path_new() {
    return new elp_path{};
}

elp_path *elp_path_copy(const elp_path *path) {
    return (elp_path *) path->copy();
}

void elp_path_free(elp_path *path) {
    delete path;
}

void elp_path_move_to(elp_path *path, double x, double y) {
    path->move_to(x, y);
}

void elp_path_line_to(elp_path *path, double x, double y) {
    path->line_to(x, y);
}

void elp_path_close_polygon(elp_path *path) {
    path->close_polygon();
}

elp_curve *elp_curve_new() {
    return new elp_curve{};
}

elp_curve *elp_curve_copy(const elp_curve *source) {
    return (elp_curve *) source->copy();
}

void elp_curve_free(elp_curve *curve) {
    delete curve;
}

void elp_curve_move_to(elp_curve *curve, double x, double y) {
    curve->move_to(x, y);
}

void elp_curve_line_to(elp_curve *curve, double x, double y) {
    curve->line_to(x, y);
}

void elp_curve_curve3(elp_curve *curve, double x_ctrl, double y_ctrl, double x_to, double y_to) {
    curve->curve3(x_ctrl, y_ctrl, x_to, y_to);
}

void elp_curve_curve4(elp_curve *curve, double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to) {
    curve->curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to);
}

void elp_curve_arc_to(elp_curve *curve, double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y) {
    curve->arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
}

void elp_curve_close_polygon(elp_curve *curve) {
    curve->close_polygon();
}

elp_dashed *elp_dashed_new(double dash_len, double gap_len) {
    elp_dashed *dashed = new elp_dashed{};
    dashed->add_dash(dash_len, gap_len);
    return dashed;
}

elp_dashed *elp_dashed_copy(const elp_dashed *path) {
    return (elp_dashed *) path->copy();
}

void elp_dashed_free(elp_dashed *path) {
    delete path;
}

void elp_dashed_add_dash(elp_dashed *path, double dash_len, double gap_len) {
    path->add_dash(dash_len, gap_len);
}

elp_object *elp_marker_symbol_by_index(int index) {
    return new_marker_symbol(index);
}

elp_object *elp_marker_symbol_by_name(const char *name) {
    return new_marker_symbol(name);
}

elp_markers *elp_markers_new(double size, elp_object *marker_symbol) {
    return new elp_markers(size, marker_symbol);
}

elp_markers *elp_markers_copy(const elp_markers *markers) {
    return (elp_markers *) markers->copy();
}

void elp_markers_free(elp_markers *markers) {
    delete markers;
}

elp_plot *elp_plot_new(unsigned int flags) {
    elp_plot *plot_struct = new elp_plot();
    if (plot_struct) {
        plot_struct->plot = new graphics::plot{flags};
        plot_struct->plot_agent = new graphics::plot_agent{};
    }
    return plot_struct;
}

void elp_plot_free(elp_plot *plot_object) {
    delete plot_object->plot;
    delete plot_object->plot_agent;
    delete plot_object;
}

void elp_plot_set_title(elp_plot *plot_object, const char *title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_title(title);
    }
    plot_update_windows_and_commit(plot_object);
}

void elp_plot_set_x_axis_title(elp_plot *plot_object, const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_x_axis_title(axis_title);
    }
    plot_update_windows_and_commit(plot_object);
}

void elp_plot_set_y_axis_title(elp_plot *plot_object, const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_y_axis_title(axis_title);
    }
    plot_update_windows_and_commit(plot_object);
}

void elp_plot_set_label_angle(elp_plot *plot_object, int axis, float angle) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_axis_labels_angle(axis == elp_x_axis ? graphics::x_axis : graphics::y_axis, angle);
    }
    plot_update_windows_and_commit(plot_object);
}

void elp_plot_enable_label_format(elp_plot *plot_object, int axis, const char *fmt) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->enable_label_format(axis == elp_x_axis ? graphics::x_axis : graphics::y_axis, fmt);
    }
    plot_update_windows_and_commit(plot_object);
}

void elp_plot_set_clip_mode(elp_plot *plot_object, bool flag) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    graphics::plot::drawing_context dc(*p);
    p->set_clip_mode(flag);
}

bool elp_plot_push_layer(elp_plot *plot_object) {
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

bool elp_plot_pop_layer(elp_plot *plot_object) {
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

void elp_plot_clear_layer(elp_plot *plot_object) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->clear_current_layer();
    }
    plot_update_windows_and_commit(plot_object);
}

void elp_plot_set_limits(elp_plot *plot_object, float x1, float y1, float x2, float y2) {
    graphics::plot *p = plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_limits(agg::rect_d(x1, y1, x2, y2));
    }
    plot_update_windows_and_commit(plot_object);
}

void elp_plot_commit_pending_draw(elp_plot *plot_object) {
    graphics::plot *p = plot_object->plot;
    graphics::plot::drawing_context dc(*p);
    p->commit_pending_draw();
}

// The plot takes implicitly the ownership of the object.
void elp_plot_add(elp_plot *plot_object, elp_object *obj, elp_color stroke_color, float stroke_width, elp_color fill_color, int flags) {
    graphics::plot *p = plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->add(obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
    }
    plot_update_windows_and_commit(plot_object);
}

int elp_plot_write_svg(elp_plot *plot_object, const char *filename, double width, double height) {
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

void elp_initialize_fonts() {
    graphics::initialize_fonts();
}
