#ifndef LIBCANVAS_H_
#define LIBCANVAS_H_

#include <cstdint>

// Following: https://google.github.io/styleguide/cppguide.html

namespace libcanvas {

struct Rectangle {
    uint8_t x1, y1, x2, y2;
};

enum Axis {
    xAxis = 0,
    yAxis
};

namespace property {
enum {
    Fill    = 1 << 0,
    Stroke  = 1 << 1,
    Outline = 1 << 2,
    Crisp   = 1 << 3,
};
}

struct Color {
    long r, g, b, a;
};

class Plot;

class Object {
public:
    ~Object();
protected:
    struct ObjectImpl;
    Object(ObjectImpl *object_impl);
    ObjectImpl *object_impl_;

    // Friend declaration needed because Plot::Add method needs to
    // have access to inner sg_object pointer, the object_impl_ field.
    friend class Plot;
};

class Path : public Object {
public:
    Path();

    void MoveTo(double x, double y);
    void LineTo(double x, double y);
    void ClosePolygon();
};

class Plot {
public:
    Plot(unsigned flags);
    ~Plot();

    void SetLimits(const Rectangle& r);
    void SetAxisLabelsAngle(const Axis& axis, float angle);
    void EnableLabelFormat(const Axis& axis, const char *fmt);

    void Add(Object& object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill|property::Stroke);

private:
    struct PlotImpl;
    PlotImpl *plot_impl_;
};

#if 0
class Window {
public:
    Window();
    Window(const char *fmt);
    ~Window();

    int Attach(Plot& plot, const char* slot_str);
    void SlotRefresh(unsigned index);
    void Start(unsigned width, unsigned height, unsigned flags);
    void Wait();

private:
    struct WindowImpl;
    WindowImpl *window_impl_;
};
#endif
}

#endif
