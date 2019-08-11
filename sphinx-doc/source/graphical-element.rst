
Graphical Elements
==================

.. cpp:namespace:: libcanvas

.. cpp:enum:: Color

  Used to express colors in RGBA format corresponds to an unsigned 32 bit integer type.

..
    .. cpp:namespace-push:: color

    .. cpp:enumerator:: Red
    .. cpp:enumerator:: Blue
    .. cpp:enumerator:: Green
    .. cpp:enumerator:: Yellow
    .. cpp:enumerator:: Black
    .. cpp:enumerator:: None = 0

.. cpp:class:: Object

  A graphical shape that can be added to a plot. An instance of the :cpp:class:`Object` cannot be directly created but it is the base class of other elements like, for example, a :cpp:class:`Path`.

.. cpp:class:: Path : public Object

  A graphical path that can contain polygonal paths, open or closed. It is a copyable and movable object.

  .. cpp:function:: Path()

    Create a new empty path.

  .. cpp:function:: void MoveTo(double x, double y)

    Move the path to the given location.

  .. cpp:function:: void LineTo(double x, double y)

    Add a straight line segment up to the given location.

  .. cpp:function:: void ClosePolygon()

    Close the current polygon.

.. cpp:class:: CurvePath : public Object

  A graphical path similar to the :cpp:class:`Path` but can additionally contain Bézier and elliptical arcs. More details about how exactly the arcs are drawn can be found from the `official SVG documentation on Paths <https://svgwg.org/specs/paths/#PathElement>`_.

  .. cpp:function:: CurvePath()

    Create an empty path.

  .. cpp:function:: void MoveTo(double x, double y)

    Move the path to the given location.

  .. cpp:function:: void LineTo(double x, double y)

    Add a straight line segment up to the given location.

  .. cpp:function:: void Curve3(double x_ctrl, double y_ctrl, double x_to, double y_to)

    Add a quadratic Bezier curve up to the point ``(x_to, y_to)`` with control point ``(x_ctrl, y_ctrl)``.

  .. cpp:function:: void Curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to)

    Add a cubic Bezier curve up to the point ``(x_to, y_to)`` with control points ``(x_ctrl1, y_ctrl1)`` and  ``(x_ctrl2, y_ctrl2)``.

  .. cpp:function:: void ArcTo(double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y)

     Add an elliptical path up to the point  ``(x, y)``. The other parameters are:

     -  ``(rx, ry)``, x and y radius of the ellipse.
     -  ``angle``, rotation angle of the ellipse's x axis.
     -  ``lare_arc_flag``, if a large angle or small angle arc should be chosen.
     -  ``bool sweep_flag``, if the arc should be drawn in the sense of increasing angles.

  .. cpp:function:: void ClosePolygon()

    Close the current polygon.

.. cpp:class:: Markers : public Path

  Like a path but draws a marker at each vertex of the path.

  .. cpp:function:: Markers(double size, Object marker_symbol)

    Create an empty Markers path that will draw markers of the specified `size`, in pixels, and using an object `marker_symbol` as a marker. The marker symbol should be an object that span a box of unit side centered on zero.

.. cpp:class:: Text : public Object

  An object that draws a text label.

  .. cpp:function:: Text(const char* text, double size = 10.0, double hjustif = 0.0, double vjustif = 0.0)

  .. cpp:function:: void SetAngle(double angle)
  .. cpp:function:: void SetPosition(double x, double y)
