// public declarations header.
#include "libcanvas_c.h"

// the following are private headers.
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

canvas_path *canvas_path_new() {
    return (canvas_path *) new graphics::path{};
}

canvas_path *canvas_path_copy(const canvas_path *path) {
    sg_object *source_obj = (sg_object *) path;
    return (canvas_path *) source_obj->copy();
}

void canvas_path_free(canvas_path *path) {
    sg_object *obj = (sg_object *) path;
    delete obj;
}

canvas_plot *canvas_plot_new(int flags) {
    struct canvas_plot *plot_struct = malloc(sizeof(struct canvas_plot));
    if (plot_struct) {
        plot_struct->plot = new graphics::plot{flags};
        plot_struct->plot_agent = new graphics::plot_agent{};
    }
    return plot_struct;
}

void canvas_plot_commit_pending_draw(canvas_plot *plot_object) {
    graphics::plot *p = plot_object->plot;
    graphics::plot::drawing_context dc(*p);
    p->commit_pending_draw();
}

static void plot_update_windows_and_commit(canvas_plot *plot_object) {
    graphics::plot_agent *agent = plot_object->plot_agent;
    agent->update_windows();
    canvas_plot_commit_pending_draw(plot_object);

}

void canvas_plot_add(canvas_plot *plot_object, canvas_object *obj, canvas_color stroke_color, float stroke_width, canvas_color fill_color, int flags) {
    graphics::plot *p = plot_object->plot;
    sg_object *sg_obj = (sg_object *) obj;
    graphics::plot::drawing_context dc(*p);
    p->add(sg_obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
    // Since the plot take the ownership null the pointer inside the object.
    // object.object_impl_ = nullptr;
    plot_update_windows_and_commit(plot_object);
}
