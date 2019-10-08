// public declarations header.
#include "libcanvas.h"

#include "agg_trans_affine.h"

// the following are private headers.
#include "libcanvas_c.h"
#include "libcanvas_c_priv.h"
#include "canvas_object.h"
#include "canvas_path.h"
#include "canvas_curve.h"
#include "path.h"
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "markers.h"
#include "canvas_svg.h"

namespace libcanvas {

Object::Object(canvas_object *object_impl): object_impl_(object_impl) {
}

Object::Object(const Object& obj) {
    object_impl_ = obj.object_impl_->copy();
}

Object::Object(Object&& obj) {
    object_impl_ = obj.object_impl_;
    obj.object_impl_ = nullptr;
}

Object::~Object() {
    delete object_impl_;
}

Object& Object::operator=(const Object& other) {
    if (this != &other) {
        delete object_impl_;
        object_impl_ = other.object_impl_->copy();
    }
    return *this;
}

Object& Object::operator=(Object&& other) {
    if (this != &other) {
        delete object_impl_;
        object_impl_ = other.object_impl_;
        other.object_impl_ = nullptr;
    }
    return *this;
}

Path::Path(): Object{(canvas_object *) new canvas_path{}} {
}

Path::Path(std::initializer_list<std::pair<double, double>> lst): Object{(canvas_object *) new canvas_path(lst)} {
}

void Path::MoveTo(double x, double y) {
    PathImpl()->move_to(x, y);
}

void Path::LineTo(double x, double y) {
    PathImpl()->line_to(x, y);
}

void Path::ClosePolygon() {
    PathImpl()->close_polygon();
}

Markers::Markers(double size, Object marker_symbol) : Path{(canvas_object *) new graphics::markers(size, (canvas_object *) marker_symbol.object_impl_)} {
    marker_symbol.object_impl_ = nullptr;
}

CurvePath::CurvePath() : Object{new canvas_curve{}} {
}

void CurvePath::MoveTo(double x, double y) {
    CurveImpl()->move_to(x, y);
}

void CurvePath::LineTo(double x, double y) {
    CurveImpl()->line_to(x, y);
}

void CurvePath::Curve3(double x_ctrl, double y_ctrl, double x_to, double y_to) {
    CurveImpl()->curve3(x_ctrl, y_ctrl, x_to, y_to);
}

void CurvePath::Curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to) {
    CurveImpl()->curve4(x_ctrl1, y_ctrl1, x_ctrl2, y_ctrl2, x_to, y_to);
}

void CurvePath::ArcTo(double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y) {
    CurveImpl()->arc_to(rx, ry, angle, large_arc_flag, sweep_flag, x, y);
}

void CurvePath::ClosePolygon() {
    CurveImpl()->close_polygon();
}

DashPath::DashPath(std::initializer_list<double> lst) : Path{(canvas_object *) new graphics::dash_path{}} {
    graphics::dash_path *dash_object = (graphics::dash_path *) object_impl_;
    double prev_len;
    bool accu = false;
    for (double len : lst) {
        if (accu) {
            dash_object->add_dash(prev_len, len);
            accu = false;
        } else {
            prev_len = len;
            accu = true;
        }
    }
}

void DashPath::AddDash(double a, double b) {
    graphics::dash_path *dash_object = (graphics::dash_path *) object_impl_;
    dash_object->add_dash(a, b);
}

Text::Text(const char* text, double size, double hjustif, double vjustif) : Object{(canvas_object *) new graphics::text(text, size, hjustif, vjustif)} {
}

void Text::SetAngle(double angle) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->angle(angle);
}

void Text::SetPosition(double x, double y) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->set_point(x, y);
}

Plot::Plot(unsigned flags) : plot_impl_{(PlotImpl *) new graphics::plot{flags}}, plot_agent_impl_{(PlotAgentImpl *) new graphics::plot_agent{}} {
}

Plot::Plot(const Plot& other) : plot_impl_{(PlotImpl *) new graphics::plot(*(const graphics::plot *) other.plot_impl_)}, plot_agent_impl_{(PlotAgentImpl *) new graphics::plot_agent{}} {
}

Plot::Plot(Plot&& other) : plot_impl_{other.plot_impl_}, plot_agent_impl_{other.plot_agent_impl_} {
    other.plot_impl_ = nullptr;
    other.plot_agent_impl_ = nullptr;
}

Plot::~Plot() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot_agent *agent = (graphics::plot_agent *) plot_agent_impl_;
    delete p;
    delete agent;
}

Plot& Plot::operator=(Plot&& other) {
    if (this != &other) {
        graphics::plot *p = (graphics::plot *) plot_impl_;
        delete p;
        plot_impl_ = other.plot_impl_;
        other.plot_impl_ = nullptr;

        graphics::plot_agent *agent = (graphics::plot_agent *) plot_agent_impl_;
        delete agent;
        plot_agent_impl_ = other.plot_agent_impl_;
        other.plot_agent_impl_ = nullptr;
    }
    return *this;
}

Plot& Plot::operator=(const Plot& other) {
    if (this != &other) {
        graphics::plot *p = (graphics::plot *) plot_impl_;
        delete p;
        const graphics::plot *other_plot = (graphics::plot *) other.plot_impl_;
        plot_impl_ = (PlotImpl *) new graphics::plot(*other_plot);
        graphics::plot_agent *agent = (graphics::plot_agent *) plot_agent_impl_;
        agent->clear();
    }
    return *this;
}

void Plot::SetTitle(const char *title) {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    canvas_plot_set_title(&cplot, title);
}

void Plot::SetXAxisTitle(const char *axis_title) {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    canvas_plot_set_x_axis_title(&cplot, axis_title);
}

void Plot::SetYAxisTitle(const char *axis_title) {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    canvas_plot_set_y_axis_title(&cplot, axis_title);
}

void Plot::SetClipMode(bool flag) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->set_clip_mode(flag);
}

void Plot::SetLimits(const Rectangle& r) {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    canvas_plot_set_limits(&cplot, r.x1, r.y1, r.x2, r.y2);
}

void Plot::SetAxisLabelsAngle(const Axis& axis, float angle) {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    canvas_plot_set_label_angle(&cplot, axis, angle);
}

void Plot::EnableLabelFormat(const Axis& axis, const char *fmt) {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    canvas_plot_enable_label_format(&cplot, axis, fmt);
}

void Plot::CommitPendingDraw() {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    canvas_plot_commit_pending_draw(&cplot);
}

void Plot::Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags) {
    if (object.object_impl_ != nullptr) {
        canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
        canvas_plot_add(&cplot, (canvas_object *) object.object_impl_, stroke_color, stroke_width, fill_color, flags);
        // Since the plot take the ownership null the pointer inside the object.
        object.object_impl_ = nullptr;
    }
}

void Plot::AddStroke(Object object, Color color, float line_width, unsigned flags) {
    Add(std::move(object), color, line_width, Color(0), flags);
}

void Plot::AddLegend(Plot legend, Plot::Placement legend_location) {
    graphics::plot *plot = (graphics::plot *) plot_impl_;
    graphics::plot *legend_plot = (graphics::plot *) legend.plot_impl_;
    plot->add_legend(legend_plot, (graphics::plot::placement_e) legend_location);
    // The plot take the ownership of the legend plot so null the pointer inside the object.
    legend.plot_impl_ = nullptr;
}

bool Plot::PushLayer() {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    return canvas_plot_push_layer(&cplot);
}

bool Plot::PopLayer() {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    return canvas_plot_pop_layer(&cplot);
}

bool Plot::WriteSvg(const char *filename, double width, double height) {
    canvas_plot cplot{(graphics::plot *) plot_impl_, (graphics::plot_agent *) plot_agent_impl_};
    return canvas_plot_write_svg(&cplot, filename, width, height);
}

Object MarkerSymbol(int n) {
    canvas_object *new_object = new_marker_symbol(n);
    return Object{(canvas_object *) new_object};
}

void InitializeFonts() {
    graphics::initialize_fonts();
}

}
