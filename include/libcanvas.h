#ifndef LIBCANVAS_H_
#define LIBCANVAS_H_

#include <initializer_list>
#include <utility>
#include <cstdint>

// Following: https://google.github.io/styleguide/cppguide.html

namespace libcanvas {

namespace color {
enum { Blue = 0x0000B4FF, Red = 0xB40000FF, Yellow = 0xF5FE00FF, None = 0 };
}

// TODO: should be move inside Window class
enum {
    WindowResize          = 1,
    WindowHwBuffer        = 2,
    WindowKeepAspectRatio = 4,
    WindowProcessAllKeys  = 8,
};

struct Rectangle {
    float x1, y1, x2, y2;
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
    uint8_t r, g, b, a;
    Color(long x): r((x >> 24) & 0xff), g((x >> 16) & 0xff), b((x >> 8) & 0xff), a(x & 0xff) {}
};

class Plot;
class Window;

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
    Path(std::initializer_list<std::pair<double, double>> lst);

    void MoveTo(double x, double y);
    void LineTo(double x, double y);
    void ClosePolygon();
};

class Polygon : public Path {
public:
    Polygon(): Path() {}
    Polygon(std::initializer_list<std::pair<double, double>> lst): Path(lst) {
        ClosePolygon();
    }
};

class Plot {
public:
    Plot(unsigned flags);
    ~Plot();

    void SetLimits(const Rectangle& r);
    void SetAxisLabelsAngle(const Axis& axis, float angle);
    void EnableLabelFormat(const Axis& axis, const char *fmt);
    void CommitPendingDraw();
    void Add(Object& object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill|property::Stroke);

    enum { ShowUnits = 1 << 0, AutoLimits = 1 << 1 };
private:
    struct PlotImpl;
    PlotImpl *plot_impl_;

    friend class Window;
};

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

void InitializeFonts();
}

#endif
