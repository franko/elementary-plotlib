// public declarations header.
#include "libelplot.h"

#include "agg_trans_affine.h"

// the following are private headers.
#include "libelplot_c.h"
#include "libelplot_c_forward.h"
#include "canvas_object.h"
#include "canvas_path.h"
#include "canvas_curve.h"
#include "canvas_dashed.h"
#include "canvas_markers.h"
#include "plot.h"
#include "plot_agent.h"
#include "window.h"
#include "markers.h"
#include "canvas_svg.h"

namespace elp {

Object::Object(elp_object *object_impl): object_impl_(object_impl) {
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

Path::Path(): Object{(elp_object *) new elp_path{}} {
}

Path::Path(std::initializer_list<std::pair<double, double>> lst): Object{(elp_object *) new elp_path(lst)} {
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

Markers::Markers(double size, Object marker_symbol) : Path{new elp_markers(size, marker_symbol.object_impl_)} {
    marker_symbol.object_impl_ = nullptr;
}

CurvePath::CurvePath() : Object{new elp_curve{}} {
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

DashPath::DashPath() : Path{new elp_dashed{}} {
}

DashPath::DashPath(std::initializer_list<double> lst) : Path{new elp_dashed{}} {
    elp_dashed *dash_object = DashedImpl();
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
    DashedImpl()->add_dash(a, b);
}

Text::Text(const char* text, double size, double hjustif, double vjustif) : Object{(elp_object *) new graphics::text(text, size, hjustif, vjustif)} {
}

void Text::SetAngle(double angle) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->angle(angle);
}

void Text::SetPosition(double x, double y) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->set_point(x, y);
}

void DeletePlotImpl(elp_plot& plot) {
    delete plot.plot;
    delete plot.plot_agent;
}

void NullPlotImpl(elp_plot& plot) {
    plot.plot = nullptr;
    plot.plot_agent = nullptr;
}

Plot::Plot(unsigned flags) : plot_impl_{new graphics::plot{flags}, new graphics::plot_agent{}} {
}

Plot::Plot(const Plot& other) : plot_impl_{new graphics::plot(*other.plot_impl_.plot), new graphics::plot_agent{}} {
}

Plot::Plot(Plot&& other) : plot_impl_{other.plot_impl_.plot, other.plot_impl_.plot_agent} {
    NullPlotImpl(other.plot_impl_);
}

Plot::~Plot() {
    DeletePlotImpl(plot_impl_);
}

Plot& Plot::operator=(Plot&& other) {
    if (this != &other) {
        DeletePlotImpl(plot_impl_);
        plot_impl_ = other.plot_impl_;
        NullPlotImpl(other.plot_impl_);
    }
    return *this;
}

Plot& Plot::operator=(const Plot& other) {
    if (this != &other) {
        delete plot_impl_.plot;
        plot_impl_.plot = new graphics::plot(*other.plot_impl_.plot);
        plot_impl_.plot_agent->clear();
    }
    return *this;
}

void Plot::SetTitle(const char *title) {
    elp_plot_set_title(&plot_impl_, title);
}

void Plot::SetXAxisTitle(const char *axis_title) {
    elp_plot_set_x_axis_title(&plot_impl_, axis_title);
}

void Plot::SetYAxisTitle(const char *axis_title) {
    elp_plot_set_y_axis_title(&plot_impl_, axis_title);
}

void Plot::SetClipMode(bool flag) {
    graphics::plot::drawing_context dc(*plot_impl_.plot);
    plot_impl_.plot->set_clip_mode(flag);
}

void Plot::SetLimits(float x1, float y1, float x2, float y2) {
    elp_plot_set_limits(&plot_impl_, x1, y1, x2, y2);
}

void Plot::SetAxisLabelsAngle(const Axis& axis, float angle) {
    elp_plot_set_label_angle(&plot_impl_, axis, angle);
}

void Plot::EnableLabelFormat(const Axis& axis, const char *fmt) {
    elp_plot_enable_label_format(&plot_impl_, axis, fmt);
}

void Plot::CommitPendingDraw() {
    elp_plot_commit_pending_draw(&plot_impl_);
}

void Plot::Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags) {
    if (object.object_impl_ != nullptr) {
        elp_plot_add(&plot_impl_, (elp_object *) object.object_impl_, stroke_color, stroke_width, fill_color, flags);
        // Since the plot take the ownership null the pointer inside the object.
        object.object_impl_ = nullptr;
    }
}

void Plot::AddStroke(Object object, Color color, float line_width, unsigned flags) {
    Add(std::move(object), color, line_width, Color(0), flags);
}

void Plot::AddLegend(Plot legend, Plot::Placement legend_location) {
    graphics::plot *legend_plot = (graphics::plot *) legend.plot_impl_.plot;
    plot_impl_.plot->add_legend(legend_plot, (graphics::plot::placement_e) legend_location);
    // The plot take the ownership of the legend plot so null the pointer inside the object.
    delete legend.plot_impl_.plot_agent;
    NullPlotImpl(legend.plot_impl_);
}

bool Plot::PushLayer() {
    return elp_plot_push_layer(&plot_impl_);
}

bool Plot::PopLayer() {
    return elp_plot_pop_layer(&plot_impl_);
}

void Plot::ClearLayer() {
    return elp_plot_clear_layer(&plot_impl_);
}

bool Plot::WriteSvg(const char *filename, double width, double height) {
    return elp_plot_write_svg(&plot_impl_, filename, width, height);
}

Object MarkerSymbol(int n) {
    elp_object *new_object = new_marker_symbol(n);
    return Object{(elp_object *) new_object};
}

void InitializeFonts() {
    graphics::initialize_fonts();
}

}
