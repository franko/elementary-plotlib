// public declarations header.
#include "elem/elem.h"

#include "agg_trans_affine.h"

// the following are private headers.
#include "elem_group.h"
#include "elem_legend.h"
#include "elem_plot.h"
#include "elem/elem_c.h"
#include "elem/elem_c_forward.h"
#include "canvas_object.h"
#include "canvas_path.h"
#include "canvas_curve.h"
#include "canvas_dashed.h"
#include "canvas_markers.h"
#include "plot.h"
#include "plot_agent.h"
#include "elem_window.h"
#include "markers.h"
#include "canvas_svg.h"

namespace elem {

Object::Object(elem_object *object_impl): object_impl_(object_impl) {
}

Object::Object(const Object& obj) {
    object_impl_ = obj.object_impl_ ? obj.object_impl_->copy() : nullptr;
    if (obj.object_impl_ && !object_impl_) {
        throw std::logic_error("geometry copy returned null");
    }
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
        Object replacement(other);
        std::swap(object_impl_, replacement.object_impl_);
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

Path::Path(): Object{(elem_object *) new elem_path{}} {
}

Path::Path(std::initializer_list<std::pair<double, double>> lst): Object{(elem_object *) new elem_path(lst)} {
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

Markers::Markers(double size, Object marker_symbol) : Path{new elem_markers(size, marker_symbol.object_impl_)} {
    marker_symbol.object_impl_ = nullptr;
}

CurvePath::CurvePath() : Object{new elem_curve{}} {
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

DashPath::DashPath() : Path{new elem_dashed{}} {
}

DashPath::DashPath(std::initializer_list<double> lst) : Path{new elem_dashed{}} {
    elem_dashed *dash_object = DashedImpl();
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

Text::Text(const char* text, double size, double hjustif, double vjustif) : Object{(elem_object *) new graphics::text(text, size, hjustif, vjustif)} {
}

void Text::SetAngle(double angle) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->angle(angle);
}

void Text::SetPosition(double x, double y) {
    graphics::text *text_object = (graphics::text *) object_impl_;
    text_object->set_point(x, y);
}

static elem_affine ToAffineC(const Affine& m) {
    elem_affine result = {m.a, m.b, m.c, m.d, m.tx, m.ty};
    return result;
}

Group::Group() : group_impl_{elem_group_new()} {
}

Group::Group(const Group& group) :
        group_impl_{group.group_impl_ ? elem_group_copy(group.group_impl_) : nullptr} {
    if (group.group_impl_ && !group_impl_) {
        throw std::logic_error("group copy returned null");
    }
}

Group::Group(Group&& group) : group_impl_{group.group_impl_} {
    group.group_impl_ = nullptr;
}

Group::~Group() {
    elem_group_free(group_impl_);
}

Group& Group::operator=(const Group& other) {
    if (this != &other) {
        Group replacement(other);
        std::swap(group_impl_, replacement.group_impl_);
    }
    return *this;
}

Group& Group::operator=(Group&& other) {
    if (this != &other) {
        elem_group_free(group_impl_);
        group_impl_ = other.group_impl_;
        other.group_impl_ = nullptr;
    }
    return *this;
}

void Group::Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags) {
    // A moved-from Group is inert: the object is released with the argument.
    if (object.object_impl_ != nullptr && group_impl_ != nullptr) {
        // Transfer before insertion: the callee owns the geometry even on failure.
        auto geometry = std::exchange(object.object_impl_, nullptr);
        elem_group_add(group_impl_, geometry, stroke_color, stroke_width, fill_color, flags);
    }
}

void Group::AddStroke(Object object, Color color, float line_width, unsigned flags) {
    Add(std::move(object), color, line_width, Color(0), flags);
}

void Group::AddGroup(Group group, Affine placement) {
    if (group.group_impl_ != nullptr && group_impl_ != nullptr) {
        // Transfer before insertion: the callee owns the subgroup even on failure.
        auto child = std::exchange(group.group_impl_, nullptr);
        elem_group_add_group(group_impl_, child, ToAffineC(placement));
    }
}

bool Group::GetScreenBounds(Bounds& bounds, Affine nominal) const {
    if (group_impl_ == nullptr) {
        return false;
    }

    elem_bounds measured;
    if (!elem_group_get_screen_bounds(group_impl_, ToAffineC(nominal), &measured)) {
        return false;
    }

    bounds = Bounds{measured.x1, measured.y1, measured.x2, measured.y2};
    return true;
}

Legend::Legend() : legend_impl_{elem_legend_new()} {
}

Legend::Legend(const Legend& legend) :
        legend_impl_{legend.legend_impl_ ? new elem_legend{*legend.legend_impl_} : nullptr} {
}

Legend::Legend(Legend&& legend) : legend_impl_{legend.legend_impl_} {
    legend.legend_impl_ = nullptr;
}

Legend::~Legend() {
    elem_legend_free(legend_impl_);
}

Legend& Legend::operator=(const Legend& other) {
    if (this != &other) {
        Legend replacement(other);
        std::swap(legend_impl_, replacement.legend_impl_);
    }
    return *this;
}

Legend& Legend::operator=(Legend&& other) {
    if (this != &other) {
        elem_legend_free(legend_impl_);
        legend_impl_ = other.legend_impl_;
        other.legend_impl_ = nullptr;
    }
    return *this;
}

void Legend::Add(Group entry) {
    if (legend_impl_ != nullptr && entry.group_impl_ != nullptr) {
        auto group = std::exchange(entry.group_impl_, nullptr);
        elem_legend_add_group(legend_impl_, group);
    }
}

Plot::Plot(Plot::Options options) : plot_impl_{new elem_plot{options.value}} {
}

Plot::Plot(const Plot& other) : plot_impl_{new elem_plot{*other.plot_impl_}} {
}

Plot::Plot(Plot&& other) : plot_impl_{new elem_plot{std::move(*other.plot_impl_)}} {
}

Plot::Plot(elem_plot *impl) : plot_impl_{impl} {
}

Plot::~Plot() {
    if (plot_impl_) {
        plot_impl_->release();
    }
}

Plot& Plot::operator=(Plot&& other) {
    if (this != &other) {
        if (plot_impl_) {
            plot_impl_->release();
        }
        plot_impl_ = other.plot_impl_;
        other.plot_impl_ = nullptr;
    }
    return *this;
}

Plot& Plot::operator=(const Plot& other) {
    if (this != &other) {
        auto replacement = new elem_plot{*other.plot_impl_};
        if (plot_impl_) {
            plot_impl_->release();
        }
        plot_impl_ = replacement;
        plot_impl_->clear_windows_links();
    }
    return *this;
}

void Plot::SetTitle(const char *title) {
    elem_plot_set_title(plot_impl_, title);
}

void Plot::SetXAxisTitle(const char *axis_title) {
    elem_plot_set_x_axis_title(plot_impl_, axis_title);
}

void Plot::SetYAxisTitle(const char *axis_title) {
    elem_plot_set_y_axis_title(plot_impl_, axis_title);
}

void Plot::SetClipMode(bool flag) {
    graphics::plot::drawing_context dc(*plot_impl_);
    plot_impl_->set_clip_mode(flag);
}

void Plot::SetLimits(float x1, float y1, float x2, float y2) {
    elem_plot_set_limits(plot_impl_, x1, y1, x2, y2);
}

void Plot::SetAxisLabelsAngle(const Axis& axis, float angle) {
    elem_plot_set_label_angle(plot_impl_, axis, angle);
}

void Plot::EnableLabelFormat(const Axis& axis, const char *fmt) {
    elem_plot_enable_label_format(plot_impl_, axis, fmt);
}

void Plot::CommitPendingDraw() {
    elem_plot_commit_pending_draw(plot_impl_);
}

void Plot::Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags) {
    if (object.object_impl_ != nullptr) {
        // Transfer before insertion: the callee owns the geometry even on failure.
        auto geometry = std::exchange(object.object_impl_, nullptr);
        elem_plot_add(plot_impl_, geometry, stroke_color, stroke_width, fill_color, flags);
    }
}

void Plot::AddStroke(Object object, Color color, float line_width, unsigned flags) {
    Add(std::move(object), color, line_width, Color(0), flags);
}

void Plot::AddGroup(Group group, Affine placement) {
    if (group.group_impl_ != nullptr) {
        // Transfer before insertion: the callee owns the group even on failure.
        auto root = std::exchange(group.group_impl_, nullptr);
        elem_plot_add_group(plot_impl_, root, ToAffineC(placement));
    }
}

void Plot::AddAnnotation(Group group, Point anchor, Affine placement) {
    if (group.group_impl_ != nullptr) {
        auto root = std::exchange(group.group_impl_, nullptr);
        elem_point anchor_c = {anchor.x, anchor.y};
        elem_plot_add_annotation(plot_impl_, root, anchor_c, ToAffineC(placement));
    }
}

void Plot::SetLegend(Legend legend, LegendLocation location) {
    if (legend.legend_impl_ != nullptr) {
        auto implementation = std::exchange(legend.legend_impl_, nullptr);
        elem_plot_set_legend(plot_impl_, implementation, static_cast<int>(location));
    }
}

void Plot::ClearLegend(LegendLocation location) {
    elem_plot_clear_legend(plot_impl_, static_cast<int>(location));
}

bool Plot::PushLayer() {
    return elem_plot_push_layer(plot_impl_);
}

bool Plot::PopLayer() {
    return elem_plot_pop_layer(plot_impl_);
}

void Plot::ClearLayer() {
    return elem_plot_clear_layer(plot_impl_);
}

bool Plot::WriteSvg(const char *filename, double width, double height) {
    return elem_plot_write_svg(plot_impl_, filename, width, height);
}

Object MarkerSymbol(int n) {
    elem_object *new_object = new_marker_symbol(n);
    return Object{(elem_object *) new_object};
}

void InitializeFonts() {
    graphics::initialize_fonts();
}

}
