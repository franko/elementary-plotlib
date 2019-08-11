// public declarations header.
#include "libcanvas.h"

// the following are private headers.
#include "sg_object.h"
#include "path.h"
#include "plot.h"
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

CustomPath::CustomPath(VertexSource& vs) : Object{(ObjectImpl *) new graphics::path_custom<VertexSource>(vs)} {
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

Plot::Plot(unsigned flags) : plot_impl_{(PlotImpl *) new graphics::plot{flags}} {
}

Plot::Plot(const Plot& other) : plot_impl_{(PlotImpl *) new graphics::plot(*(const graphics::plot *) other.plot_impl_)} {
}

Plot::Plot(Plot&& other) : plot_impl_{other.plot_impl_} {
    other.plot_impl_ = nullptr;
}

Plot::~Plot() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    delete p;
}

void Plot::SetTitle(const char *title) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->set_title(title);
}

void Plot::SetXAxisTitle(const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->set_x_axis_title(axis_title);
}

void Plot::SetClipMode(bool flag) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->set_clip_mode(flag);
}

void Plot::SetLimits(const Rectangle& r) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->set_limits(agg::rect_d(r.x1, r.y1, r.x2, r.y2));
}

void Plot::SetAxisLabelsAngle(const Axis& axis, float angle) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->set_axis_labels_angle(axis == xAxis ? graphics::x_axis : graphics::y_axis, angle);
}

void Plot::EnableLabelFormat(const Axis& axis, const char *fmt) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    p->enable_label_format(axis == xAxis ? graphics::x_axis : graphics::y_axis, fmt);
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
    graphics::plot::drawing_context dc(*p);
    return p->push_layer();
}

bool Plot::PopLayer() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    graphics::plot::drawing_context dc(*p);
    return p->pop_layer();
}

#ifdef WIN32
typedef window_gen<window_win32> PlatformWindowImpl;
#else
typedef window_gen<xwindow> PlatformWindowImpl;
#endif

Window::Window() : window_impl_{(WindowImpl *) new PlatformWindowImpl()} {
}

Window::Window(const char *fmt) : window_impl_{(WindowImpl *) new PlatformWindowImpl(fmt)} {
}

Window::~Window() {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    delete win;
}

int Window::Attach(Plot& plot, const char* slot_str) {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    graphics::plot *p = (graphics::plot *) plot.plot_impl_;
    return win->attach(p, slot_str);
}

void Window::SlotRefresh(unsigned index) {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    win->slot_refresh(index);
}

void Window::Start(unsigned width, unsigned height, unsigned flags) {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    win->start(width, height, flags);
}

void Window::Wait() {
    PlatformWindowImpl *win = (PlatformWindowImpl *) window_impl_;
    win->wait();
}

Object MarkerSymbol(int n) {
    sg_object *new_object = new_marker_symbol(n);
    return Object{(Object::ObjectImpl *) new_object};
}

void InitializeFonts() {
    graphics::initialize_fonts();
}

}
