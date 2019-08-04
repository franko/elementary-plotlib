#ifndef LIBCANVAS_H_
#define LIBCANVAS_H_

#include <initializer_list>
#include <utility>
#include <cstdint>

// Following: https://google.github.io/styleguide/cppguide.html

namespace libcanvas {

typedef uint32_t Color;

namespace color {
enum : uint32_t { Red = 0xB40000FF, Green = 0x00CC00ff, Blue = 0x0000B4FF, Yellow = 0xF5FE00FF, Black = 0x000000FF, White = 0xFFFFFFFF, None = 0 };

inline Color RGBA(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
    return ((r & 0xff) << 24) | ((g & 0xff) << 16) | ((b & 0xff) << 8) | (a & 0xff);
}

inline Color RGB(unsigned int r, unsigned int g, unsigned int b) {
    return RGBA(r, g, b, 0xff);
}
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

class Plot;
class Window;

class Object {
public:
    Object(const Object& obj);
    Object(Object&& obj);
    ~Object();
protected:
    struct ObjectImpl;
    Object(ObjectImpl *object_impl);
    ObjectImpl *object_impl_;

    // Friend declaration needed because Plot::Add method needs to
    // have access to inner sg_object pointer, the object_impl_ field.
    friend class Plot;
    friend class Markers;

    // Needs to create an object using protected constructor.
    friend Object MarkerSymbol(int n);
};

Object MarkerSymbol(int index);

class Path : public Object {
public:
    Path();
    Path(const Path& path): Object(path) { }
    Path(Path&& path): Object(path) { }
    Path(std::initializer_list<std::pair<double, double>> lst);

    void MoveTo(double x, double y);
    void LineTo(double x, double y);
    void ClosePolygon();

protected:
    Path(ObjectImpl *object_impl) : Object(object_impl) { }
};

class Polygon : public Path {
public:
    Polygon(): Path() {}
    Polygon(const Polygon& path): Path(path) { }
    Polygon(Polygon&& path): Path(path) { }
    Polygon(std::initializer_list<std::pair<double, double>> lst): Path(lst) {
        ClosePolygon();
    }
};

class Markers : public Path {
public:
    Markers(double size, Object marker_symbol);
    Markers(const Markers& obj): Path(obj) { }
    Markers(Markers&& obj): Path(obj) { }
};

class Plot {
public:
    Plot(unsigned flags);
    ~Plot();

    void SetTitle(const char *title);
    void SetXAxisTitle(const char *axis_title);
    void SetClipMode(bool flag);
    void SetLimits(const Rectangle& r);
    void SetAxisLabelsAngle(const Axis& axis, float angle);
    void EnableLabelFormat(const Axis& axis, const char *fmt);
    void CommitPendingDraw();
    void Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill|property::Stroke);
    bool PushLayer();
    bool PopLayer();

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
