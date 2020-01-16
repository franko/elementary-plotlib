/*
** Libelplot -- a plotting library for C++. https://github.com/franko/libcanvas
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

#ifndef LIBELPLOT_CPP_H_
#define LIBELPLOT_CPP_H_

#include <initializer_list>
#include <utility>
#include <cstdint>

// Following: https://google.github.io/styleguide/cppguide.html

// Forward declaration of C API objects.
#include "libelplot_c_forward.h"

namespace FX {
    class FXElemWindow;
}

namespace elp {

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

struct Rectangle {
    float x1, y1, x2, y2;
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

class Plot;
class Window;

class Object {
public:
    Object() : object_impl_(nullptr) { }
    Object(const Object& obj);
    Object(Object&& obj);
    ~Object();

    Object& operator=(const Object& that);
    Object& operator=(Object&& that);
protected:
    Object(elp_object *object_impl);
    elp_object *object_impl_;

    // Friend declaration needed because Plot::Add method needs to
    // have access to inner elp_object pointer, the object_impl_ field.
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
    Path(elp_object *object_impl) : Object(object_impl) { }
    elp_path *PathImpl() { return (elp_path *) object_impl_; }
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
    CurvePath(elp_object *object) : Object(object) { }
    elp_curve *CurveImpl() { return (elp_curve *) object_impl_; }
};

class DashPath : public Path {
public:
    DashPath(std::initializer_list<double> lst);
    DashPath(const DashPath& path): Path(path) { }
    DashPath(DashPath&& path): Path(path) { }

    void AddDash(double a, double b);

protected:
    DashPath(elp_object *object_impl) : Path(object_impl) { }
    elp_dashed *DashedImpl() { return (elp_dashed *) object_impl_; }
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

class Plot {
public:
    enum { ShowUnits = 1 << 0, AutoLimits = 1 << 1 };
    enum Placement { Right = 0, Left = 1, Bottom = 2, Top = 3 };

    Plot() : Plot(ShowUnits | AutoLimits) { }
    Plot(unsigned flags);
    Plot(const Plot& other);
    Plot(Plot&& other);
    ~Plot();

    Plot& operator=(Plot&& other);
    Plot& operator=(const Plot& other);

    void SetTitle(const char *title);
    void SetXAxisTitle(const char *axis_title);
    void SetYAxisTitle(const char *axis_title);
    void SetClipMode(bool flag);
    void SetLimits(const Rectangle& r);
    void SetAxisLabelsAngle(const Axis& axis, float angle);
    void EnableLabelFormat(const Axis& axis, const char *fmt);
    void CommitPendingDraw();
    void Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill|property::Stroke);
    void AddStroke(Object object, Color color, float line_width, unsigned flags = property::Stroke);
    bool PushLayer();
    bool PopLayer();
    void ClearLayer();
    void AddLegend(Plot legend, Placement location);
    bool WriteSvg(const char *filename, double width, double height);
private:
    void UpdateWindowsAndCommitChanges();

    elp_plot plot_impl_;

    friend class Window;
    friend class FX::FXElemWindow;
};

class Window {
public:
    Window();
    Window(const char *layout);
    Window(elp_window *window_impl);
    ~Window();

    int Attach(Plot& plot, const char* slot_str);
    void SetLayout(const char *fmt);
    void SlotRefresh(unsigned index);
    void Start(unsigned width, unsigned height, unsigned flags);
    void Wait();

private:
    elp_window *window_impl_;
};

void InitializeFonts();
}

#endif
