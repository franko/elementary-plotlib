// public declarations header.
#include "libcanvas_c.h"

// the following are private headers.
#include "canvas_svg.h"
#include "sg_object.h"
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
    const sg_object *source_obj = (sg_object *) obj;
    return (canvas_object *) source_obj->copy();
}

void canvas_object_free(canvas_object *obj) {
    sg_object *sg_obj = (sg_object *) obj;
    delete sg_obj;
}

canvas_path *canvas_path_new() {
    return (canvas_path *) new graphics::path{};
}

canvas_path *canvas_path_copy(const canvas_path *path) {
    sg_object *source_obj = (sg_object *) path;
    return (canvas_path *) source_obj->copy();
}

void canvas_path_free(canvas_path *path) {
    canvas_object_free(canvas_object(path));
}

void canvas_path_move_to(canvas_path *path_object, double x, double y) {
    graphics::path *path = (graphics::path *) path_object;
    path->move_to(x, y);
}

void canvas_path_line_to(canvas_path *path_object, double x, double y) {
    graphics::path *path = (graphics::path *) path_object;
    path->line_to(x, y);
}

void canvas_path_close_polygon(canvas_path *path_object) {
    graphics::path *path = (graphics::path *) path_object;
    path->close_polygon();
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
    sg_object *sg_obj = (sg_object *) obj;
    {
        graphics::plot::drawing_context dc(*p);
        p->add(sg_obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
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
