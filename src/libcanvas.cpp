// public declarations header.
#include "libcanvas.h"

// the following are private headers.
#include "sg_object.h"
#include "path.h"
#include "plot.h"

static agg::rgba8 ColorToRgba8(const libcanvas::Color& c) {
    return agg::rgba8(c.r, c.g, c.b, c.a);
}

namespace libcanvas {

Object::Object(Object::ObjectImpl *object_impl): object_impl_(object_impl) {
}

Object::~Object() {
    sg_object *obj = (sg_object *) object_impl_;
    delete obj;
};

Path::Path(): Object{(ObjectImpl *) new graphics::path{}} {
}

void Path::MoveTo(double x, double y) {
    graphics::path *path_object = (graphics::path *) object_impl_;
    path_object->move_to(x, y);
}

void Path::LineTo(double x, double y) {
    graphics::path *path_object = (graphics::path *) object_impl_;
    path_object->line_to(x, y);
}

void Path::ClosePolygon() {
    graphics::path *path_object = (graphics::path *) object_impl_;
    path_object->close_polygon();
}

Plot::Plot(unsigned flags) : plot_impl_{(PlotImpl *) new graphics::plot{flags}} {
}

Plot::~Plot() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    delete p;
}

void Plot::SetLimits(const Rectangle& r) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    p->set_limits(agg::rect_d(r.x1, r.y1, r.x2, r.y2));
}

void Plot::SetAxisLabelsAngle(const Axis& axis, float angle) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    p->set_axis_labels_angle(axis == xAxis ? graphics::x_axis : graphics::y_axis, angle);
}

void Plot::EnableLabelFormat(const Axis& axis, const char *fmt) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    p->enable_label_format(axis == xAxis ? graphics::x_axis : graphics::y_axis, fmt);
}

void Plot::Add(Object& object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    sg_object *sg_obj = (sg_object *) (object.object_impl_);
    p->add(sg_obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
}

}
