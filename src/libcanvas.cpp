// public declarations header.
#include "libcanvas.h"

// the following are private headers.
#include "sg_object.h"
#include "path.h"
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "markers.h"

static agg::rgba8 ColorToRgba8(const libcanvas::Color& c) {
    return agg::rgba8((c >> 24) & 0xff, (c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
}

namespace libcanvas {

Object::Object(Object::ObjectImpl *object_impl): object_impl_(object_impl) {
}

Object::~Object() {
    sg_object *obj = (sg_object *) object_impl_;
    delete obj;
};

Object::Object(const Object& obj) {
    sg_object *source_obj = (sg_object *) obj.object_impl_;
    sg_object *new_obj = source_obj->copy();
    object_impl_ = (ObjectImpl *) new_obj;
}

Object::Object(Object&& obj) {
    sg_object *source_obj = (sg_object *) obj.object_impl_;
    obj.object_impl_ = nullptr;
    object_impl_ = (ObjectImpl *) source_obj;
}

Object& Object::operator=(const Object& other) {
    if (this != &other) {
        sg_object *obj = (sg_object *) object_impl_;
        delete obj;
        const sg_object *other_obj = (sg_object *) other.object_impl_;
        object_impl_ = (ObjectImpl *) other_obj->copy();
    }
    return *this;
}

Object& Object::operator=(Object&& other) {
    if (this != &other) {
        sg_object *obj = (sg_object *) object_impl_;
        delete obj;
        object_impl_ = other.object_impl_;
        other.object_impl_ = nullptr;
    }
    return *this;
}

Path::Path(): Object{(ObjectImpl *) new graphics::path{}} {
}

Path::Path(std::initializer_list<std::pair<double, double>> lst): Object{(ObjectImpl *) new graphics::path(lst)} {
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

Markers::Markers(double size, Object marker_symbol) : Path{(Object::ObjectImpl *) new graphics::markers(size, (sg_object *) marker_symbol.object_impl_)} {
    marker_symbol.object_impl_ = nullptr;
}

CurvePath::CurvePath() : Object{(ObjectImpl *) new graphics::curve_path{}} {
}

void CurvePath::MoveTo(double x, double y) {
    graphics::curve_path *path_object = (graphics::curve_path *) object_impl_;
    path_object->move_to(x, y);
}

void CurvePath::LineTo(double x, double y) {
    graphics::curve_path *path_object = (graphics::curve_path *) object_impl_;
    path_object->line_to(x, y);
}

void CurvePath::Curve3(double x_ctrl, double y_ctrl, double x_to, double y_to) {
    graphics::curve_path *path_object = (graphics::curve_path *) object_impl_;
    path_object->curve3(x_ctrl, y_ctrl, x_to, y_to);
}

void CurvePath::Curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to) {
    graphics::curve_path *path_object = (graphics::curve_path *) object_impl_;
    path_object->curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to);
}

void CurvePath::ArcTo(double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y) {
    graphics::curve_path *path_object = (graphics::curve_path *) object_impl_;
    path_object->arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
}

void CurvePath::ClosePolygon() {
    graphics::curve_path *path_object = (graphics::curve_path *) object_impl_;
    path_object->close_polygon();
}

DashPath::DashPath() : Path{(ObjectImpl *) new graphics::dash_path{}} {
}

void DashPath::AddDash(double a, double b) {
    graphics::dash_path *dash_object = (graphics::dash_path *) object_impl_;
    dash_object->add_dash(a, b);
}

Text::Text(const char* text, double size, double hjustif, double vjustif) : Object{(ObjectImpl *) new graphics::text(text, size, hjustif, vjustif)} {
}

void Text::SetAngle(double angle) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->angle(angle);
}

void Text::SetPosition(double x, double y) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->set_point(x, y);
}

Plot::Plot(unsigned flags) : plot_impl_{(PlotImpl *) new graphics::plot{flags}}, plot_agent_{new PlotAgent{}} {
}

Plot::Plot(const Plot& other) : plot_impl_{(PlotImpl *) new graphics::plot(*(const graphics::plot *) other.plot_impl_)}, plot_agent_{new PlotAgent{}} {
}

Plot::Plot(Plot&& other) : plot_impl_{other.plot_impl_}, plot_agent_{other.plot_agent_} {
    other.plot_impl_ = nullptr;
    other.plot_agent_ = nullptr;
}

Plot::~Plot() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    delete p;
    delete plot_agent_;
}

Plot& Plot::operator=(Plot&& other) {
    if (this != &other) {
        graphics::plot *p = (graphics::plot *) plot_impl_;
        delete p;
        plot_impl_ = other.plot_impl_;
        other.plot_impl_ = nullptr;

        delete plot_agent_;
        plot_agent_ = other.plot_agent_;
        other.plot_agent_ = nullptr;
    }
    return *this;
}

Plot& Plot::operator=(const Plot& other) {
    if (this != &other) {
        graphics::plot *p = (graphics::plot *) plot_impl_;
        delete p;
        const graphics::plot *other_plot = (graphics::plot *) other.plot_impl_;
        plot_impl_ = (PlotImpl *) new graphics::plot(*other_plot);
        plot_agent_->Clear();
    }
    return *this;
}

void Plot::SetTitle(const char *title) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_title(title);
    }
    UpdateWindowsAndCommitChanges();
}

void Plot::SetXAxisTitle(const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_x_axis_title(axis_title);
    }
    UpdateWindowsAndCommitChanges();
}

void Plot::SetYAxisTitle(const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_y_axis_title(axis_title);
    }
    UpdateWindowsAndCommitChanges();
}

void Plot::SetClipMode(bool flag) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->set_clip_mode(flag);
}

void Plot::SetLimits(const Rectangle& r) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_limits(agg::rect_d(r.x1, r.y1, r.x2, r.y2));
    }
    UpdateWindowsAndCommitChanges();
}

void Plot::SetAxisLabelsAngle(const Axis& axis, float angle) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    {
        graphics::plot::drawing_context dc(*p);
        p->set_axis_labels_angle(axis == xAxis ? graphics::x_axis : graphics::y_axis, angle);
    }
    UpdateWindowsAndCommitChanges();
}

void Plot::EnableLabelFormat(const Axis& axis, const char *fmt) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    {
        graphics::plot::drawing_context dc(*p);
        p->enable_label_format(axis == xAxis ? graphics::x_axis : graphics::y_axis, fmt);
    }
    UpdateWindowsAndCommitChanges();
}

void Plot::CommitPendingDraw() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->commit_pending_draw();
}

void Plot::Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    sg_object *sg_obj = (sg_object *) (object.object_impl_);
    if (sg_obj != nullptr) {
        graphics::plot::drawing_context dc(*p);
        p->add(sg_obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
        // Since the plot take the ownership null the pointer inside the object.
        object.object_impl_ = nullptr;
    }
    UpdateWindowsAndCommitChanges();
}

void Plot::AddStroke(Object object, Color color, float line_width, unsigned flags) {
    Add(object, color, line_width, Color(0), flags);
}

void Plot::AddLegend(Plot legend, Plot::Placement legend_location) {
    graphics::plot *plot = (graphics::plot *) plot_impl_;
    graphics::plot *legend_plot = (graphics::plot *) legend.plot_impl_;
    plot->add_legend(legend_plot, (graphics::plot::placement_e) legend_location);
    // The plot take the ownership of the legend plot so null the pointer inside the object.
    legend.plot_impl_ = nullptr;
}

bool Plot::PushLayer() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    bool success = false;
    {
        graphics::plot::drawing_context dc(*p);
        success = p->push_layer();
    }
    if (success) {
        UpdateWindowsAndCommitChanges();
    }
    return success;
}

bool Plot::PopLayer() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    bool success;
    {
        graphics::plot::drawing_context dc(*p);
        success = p->pop_layer();
    }
    if (success) {
        UpdateWindowsAndCommitChanges();
    }
    return success;
}

void Plot::UpdateWindowsAndCommitChanges() {
    plot_agent_->UpdateWindows();
    CommitPendingDraw();
}

Object MarkerSymbol(int n) {
    sg_object *new_object = new_marker_symbol(n);
    return Object{(Object::ObjectImpl *) new_object};
}

void InitializeFonts() {
    graphics::initialize_fonts();
}

}
