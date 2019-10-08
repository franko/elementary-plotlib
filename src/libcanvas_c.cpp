// public declarations header.
#include "libcanvas_c.h"

// the following are private headers.
#include "canvas_svg.h"
#include "canvas_object.h"
#include "canvas_path.h"
#include "canvas_curve.h"
#include "path.h"
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "markers.h"
#include "libcanvas_c_priv.h"

static agg::rgba8 ColorToRgba8(const canvas_color& c) {
    return agg::rgba8((c >> 24) & 0xff, (c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
}

static void plot_update_windows_and_commit(canvas_plot *plot_object) {
    graphics::plot_agent *agent = plot_object->plot_agent;
    agent->update_windows();
    canvas_plot_commit_pending_draw(plot_object);
}

canvas_object *canvas_object_copy(const canvas_object *obj) {
    return obj->copy();
}

void canvas_object_free(canvas_object *obj) {
    delete obj;
}

canvas_path *canvas_path_new() {
    return new canvas_path{};
}

canvas_path *canvas_path_copy(const canvas_path *path) {
    return (canvas_path *) path->copy();
}

void canvas_path_free(canvas_path *path) {
    delete path;
}

void canvas_path_move_to(canvas_path *path, double x, double y) {
    path->move_to(x, y);
}

void canvas_path_line_to(canvas_path *path, double x, double y) {
    path->line_to(x, y);
}

void canvas_path_close_polygon(canvas_path *path) {
    path->close_polygon();
}

canvas_curve *canvas_curve_new() {
    return new canvas_curve{};
}

canvas_curve *canvas_curve_copy(const canvas_curve *source) {
    return (canvas_curve *) source->copy();
}

void canvas_curve_free(canvas_curve *curve) {
    delete curve;
}

void canvas_curve_move_to(canvas_curve *curve, double x, double y) {
    curve->move_to(x, y);
}

void canvas_curve_line_to(canvas_curve *curve, double x, double y) {
    curve->line_to(x, y);
}

void canvas_curve_curve3(canvas_curve *curve, double x_ctrl, double y_ctrl, double x_to, double y_to) {
    curve->curve3(x_ctrl, y_ctrl, x_to, y_to);
}

void canvas_curve_curve4(canvas_curve *curve, double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to) {
    curve->curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to);
}

void canvas_curve_arc_to(canvas_curve *curve, double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y) {
    curve->arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
}

void canvas_curve_close_polygon(canvas_curve *curve) {
    curve->close_polygon();
}

canvas_plot *canvas_plot_new(unsigned int flags) {
    canvas_plot *plot_struct = new canvas_plot();
    if (plot_struct) {
        plot_struct->plot = new graphics::plot{flags};
        plot_struct->plot_agent = new graphics::plot_agent{};
    }
    return plot_struct;
}

void canvas_plot_free(canvas_plot *plot_object) {
    delete plot_object->plot;
    delete plot_object->plot_agent;
    delete plot_object;
}

void canvas_plot_set_title(canvas_plot *plot_object, const char *title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_title(title);
    }
    plot_update_windows_and_commit(plot_object);
}

void canvas_plot_set_x_axis_title(canvas_plot *plot_object, const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_x_axis_title(axis_title);
    }
    plot_update_windows_and_commit(plot_object);
}

void canvas_plot_set_y_axis_title(canvas_plot *plot_object, const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_y_axis_title(axis_title);
    }
    plot_update_windows_and_commit(plot_object);
}

void canvas_plot_set_label_angle(canvas_plot *plot_object, int axis, float angle) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_axis_labels_angle(axis == canvas_x_axis ? graphics::x_axis : graphics::y_axis, angle);
    }
    plot_update_windows_and_commit(plot_object);
}

void canvas_plot_enable_label_format(canvas_plot *plot_object, int axis, const char *fmt) {
    graphics::plot *p = (graphics::plot *) plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->enable_label_format(axis == canvas_x_axis ? graphics::x_axis : graphics::y_axis, fmt);
    }
    plot_update_windows_and_commit(plot_object);
}

bool canvas_plot_push_layer(canvas_plot *plot_object) {
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

bool canvas_plot_pop_layer(canvas_plot *plot_object) {
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

void canvas_plot_set_limits(canvas_plot *plot_object, float x1, float y1, float x2, float y2) {
    graphics::plot *p = plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_limits(agg::rect_d(x1, y1, x2, y2));
    }
    plot_update_windows_and_commit(plot_object);
}

void canvas_plot_commit_pending_draw(canvas_plot *plot_object) {
    graphics::plot *p = plot_object->plot;
    graphics::plot::drawing_context dc(*p);
    p->commit_pending_draw();
}

// The plot takes implicitly the ownership of the object.
void canvas_plot_add(canvas_plot *plot_object, canvas_object *obj, canvas_color stroke_color, float stroke_width, canvas_color fill_color, int flags) {
    graphics::plot *p = plot_object->plot;
    {
        graphics::plot::drawing_context dc(*p);
        p->add(obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
    }
    plot_update_windows_and_commit(plot_object);
}

int canvas_plot_write_svg(canvas_plot *plot_object, const char *filename, double width, double height) {
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

void canvas_initialize_fonts() {
    graphics::initialize_fonts();
}
