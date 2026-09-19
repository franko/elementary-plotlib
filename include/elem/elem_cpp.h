/*
** Elementary Plot -- a plotting library for C++. https://github.com/franko/elementary-plotlib
**
** Copyright (C) 2018-2019 Francesco Abbate. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#ifndef ELEM_PLOT_CPP_H_
#define ELEM_PLOT_CPP_H_

#include <cmath>
#include <initializer_list>
#include <utility>
#include <cstdint>

// Following: https://google.github.io/styleguide/cppguide.html

// Forward declaration of C API objects.
#include "elem/elem_c_forward.h"

namespace FX {
    class FXElemWindow;
}

namespace elem {

typedef uint32_t Color;

namespace color {
enum : uint32_t { Red = 0xB40000FF, Green = 0x00CC00ff, Blue = 0x0000B4FF, Yellow = 0xF5FE00FF, Black = 0x000000FF, Gray = 0xBBBBBBFF, White = 0xFFFFFFFF, None = 0 };

inline Color RGBA(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
    return ((r & 0xff) << 24) | ((g & 0xff) << 16) | ((b & 0xff) << 8) | (a & 0xff);
}

inline Color RGB(unsigned int r, unsigned int g, unsigned int b) {
    return RGBA(r, g, b, 0xff);
}
}

enum {
    WindowResize          = 1,
    WindowHwBuffer        = 2,
    WindowKeepAspectRatio = 4,
    WindowProcessAllKeys  = 8,
};

enum Axis {
    xAxis = 0,
    yAxis = 1,
};

namespace property {
enum {
    Fill    = 1 << 0,
    Stroke  = 1 << 1,
    Outline = 1 << 2,
    Crisp   = 1 << 3,
};
}

class Group;
class Plot;
class Window;

struct Point {
    double x, y;
};

struct Bounds {
    double x1, y1, x2, y2;

    double Width() const { return x2 - x1; }
    double Height() const { return y2 - y1; }
};

// An affine transform, with x' = a*x + c*y + tx and y' = b*x + d*y + ty.
// Rotation angles are in radians.
class Affine {
public:
    Affine() : a(1), b(0), c(0), d(1), tx(0), ty(0) { }
    Affine(double a, double b, double c, double d, double tx, double ty) :
        a(a), b(b), c(c), d(d), tx(tx), ty(ty) { }

    static Affine Identity() { return Affine{}; }

    static Affine Translation(double tx, double ty) {
        return Affine{1, 0, 0, 1, tx, ty};
    }

    static Affine Rotation(double angle) {
        const double cos_a = std::cos(angle), sin_a = std::sin(angle);
        return Affine{cos_a, sin_a, -sin_a, cos_a, 0, 0};
    }

    static Affine Scale(double sx, double sy) {
        return Affine{sx, 0, 0, sy, 0, 0};
    }

    static Affine Scale(double s) { return Scale(s, s); }

    // Apply this transform before next.
    Affine Then(const Affine& next) const {
        return Affine{
            next.a * a  + next.c * b,
            next.b * a  + next.d * b,
            next.a * c  + next.c * d,
            next.b * c  + next.d * d,
            next.a * tx + next.c * ty + next.tx,
            next.b * tx + next.d * ty + next.ty,
        };
    }

    double a, b, c, d, tx, ty;
};

class Object {
public:
    Object() : object_impl_(nullptr) { }
    Object(const Object& obj);
    Object(Object&& obj);
    ~Object();

    Object& operator=(const Object& that);
    Object& operator=(Object&& that);
protected:
    Object(elem_object *object_impl);
    elem_object *object_impl_;

    // Friend declaration needed because the Plot::Add and Group::Add methods
    // need to have access to inner elem_object pointer, the object_impl_ field.
    friend class Plot;
    friend class Group;
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
    Path(elem_object *object_impl) : Object(object_impl) { }
    elem_path *PathImpl() { return (elem_path *) object_impl_; }
};

// FIXME to be changed or improved: it would be better if CurvePath was
// directly inheriting from Path.
class CurvePath : public Object {
public:
    CurvePath();
    CurvePath(const CurvePath& path): Object(path) { }
    CurvePath(CurvePath&& path): Object(path) { }

    void MoveTo(double x, double y);
    void LineTo(double x, double y);
    void Curve3(double x_ctrl, double y_ctrl, double x_to, double y_to);
    void Curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to);
    void ArcTo(double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y);
    void ClosePolygon();

protected:
    CurvePath(elem_object *object) : Object(object) { }
    elem_curve *CurveImpl() { return (elem_curve *) object_impl_; }
};

class DashPath : public Path {
public:
    DashPath();
    DashPath(std::initializer_list<double> lst);
    DashPath(const DashPath& path): Path(path) { }
    DashPath(DashPath&& path): Path(path) { }

    void AddDash(double a, double b);

protected:
    DashPath(elem_object *object_impl) : Path(object_impl) { }
    elem_dashed *DashedImpl() { return (elem_dashed *) object_impl_; }
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

class Text : public Object {
public:
    Text(const char* text, double size = 10.0, double hjustif = 0.0, double vjustif = 0.0);
    Text(const Text& source) : Object(source) { }
    Text(const Text&& source) : Object(source) { }

    void SetAngle(double angle);
    void SetPosition(double x, double y);
};

// A reusable collection of independently styled drawing operations, positioned
// in the group's own local coordinate system. Placing a group into a plot
// transforms child positions and path geometry, while stroke widths, dash
// lengths, glyph sizes, and marker symbols stay screen-sized.
//
// Insertion takes a snapshot: passing an lvalue copies and passing a moved
// value transfers ownership, so editing a builder afterwards does not change
// what the plot draws.
class Group {
public:
    Group();
    Group(const Group& group);
    Group(Group&& group);
    ~Group();

    Group& operator=(const Group& that);
    Group& operator=(Group&& that);

    void Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill|property::Stroke);
    void AddStroke(Object object, Color color, float line_width, unsigned flags = property::Stroke);
    // The placement applies once, to the inserted subgroup root.
    void AddGroup(Group group, Affine placement = Affine{});

    // Return the painted bounds in logical screen coordinates. The nominal
    // transform maps group-parent coordinates to screen coordinates and is
    // composed after the group's own transform. Returns false for an empty or
    // completely non-painted group.
    bool GetScreenBounds(Bounds& bounds, Affine nominal = Affine{}) const;

private:
    elem_group *group_impl_;

    // Plot::AddGroup and Plot::AddAnnotation need the inner elem_group pointer.
    friend class Plot;
};

class Plot {
public:
    enum { ShowUnits = 1 << 0, AutoLimits = 1 << 1, ClipRegion = 1 << 2 };
    enum Placement { Right = 0, Left = 1, Bottom = 2, Top = 3 };

    struct Options {
        Options() : value(ShowUnits | AutoLimits) { }
        Options(unsigned flags) : value(flags) { }
        unsigned value;
    };

    Plot(Options flags = Options{});
    Plot(const Plot& other);
    Plot(Plot&& other);
    ~Plot();

    Plot& operator=(Plot&& other);
    Plot& operator=(const Plot& other);

    void SetTitle(const char *title);
    void SetXAxisTitle(const char *axis_title);
    void SetYAxisTitle(const char *axis_title);
    void SetClipMode(bool flag);
    void SetLimits(float x1, float y1, float x2, float y2);
    void SetAxisLabelsAngle(const Axis& axis, float angle);
    void EnableLabelFormat(const Axis& axis, const char *fmt);
    void CommitPendingDraw();
    void Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill|property::Stroke);
    void AddStroke(Object object, Color color, float line_width, unsigned flags = property::Stroke);
    // Local geometry becomes data geometry: it follows the plot's coordinate
    // scaling and contributes to automatic limits.
    void AddGroup(Group group, Affine placement = Affine{});
    // The anchor is in data coordinates; the local layout and the placement
    // offset are in logical screen units. Annotations never change the limits.
    void AddAnnotation(Group group, Point anchor, Affine placement = Affine{});
    bool PushLayer();
    bool PopLayer();
    void Show(unsigned width, unsigned height, unsigned flags);
    void ClearLayer();
    void SetLegendPlot(Plot legend, Placement location);
    Plot GetLegendPlot(Placement legend_location);
    bool WriteSvg(const char *filename, double width, double height);
    void Wait();
private:
    Plot(elem_plot *impl);
    void UpdateWindowsAndCommitChanges();

    elem_plot *plot_impl_;

    friend class Window;
    friend class FX::FXElemWindow;
};

class Window {
public:
    Window();
    Window(const char *layout);
    Window(elem_window *window_impl);
    ~Window();

    int Attach(Plot& plot, const char* slot_str);
    void SetLayout(const char *fmt);
    void SlotRefresh(unsigned index);
    void Start(unsigned width, unsigned height, unsigned flags);
    void Wait();
    void Close();

private:
    elem_window *window_impl_;
};

void InitializeFonts();

int InitializeAndRun(int (*user_main)());
}

#endif
