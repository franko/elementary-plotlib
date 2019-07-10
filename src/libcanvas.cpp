// public declarations header.
#include "libcanvas.h"

// the following are private headers.
#include "sg_object.h"
#include "path.h"
#include "plot.h"
#include "window.h"

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

Plot::Plot(unsigned flags) : plot_impl_{(PlotImpl *) new graphics::plot{flags}} {
}

Plot::~Plot() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    delete p;
}

void Plot::SetTitle(const char *title) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    p->set_title(title);
}

void Plot::SetXAxisTitle(const char *axis_title) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    p->set_x_axis_title(axis_title);
}

void Plot::SetClipMode(bool flag) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    p->set_clip_mode(flag);
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

void Plot::CommitPendingDraw() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    p->commit_pending_draw();
}

void Plot::Add(Object& object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags) {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    sg_object *sg_obj = (sg_object *) (object.object_impl_);
    if (sg_obj != nullptr) {
        p->add(sg_obj, ColorToRgba8(stroke_color), stroke_width, ColorToRgba8(fill_color), flags);
        // Since the plot take the ownership null the pointer inside the object.
        object.object_impl_ = nullptr;
    }
}

bool Plot::PushLayer() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
    return p->push_layer();
}

bool Plot::PopLayer() {
    graphics::plot *p = (graphics::plot *) plot_impl_;
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

void InitializeFonts() {
    graphics::initialize_fonts();
}

}
