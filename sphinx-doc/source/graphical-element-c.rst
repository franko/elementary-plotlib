
Graphical Elements - C API
==========================

.. c:type:: canvas_color 

  Used to express colors in RGBA format. Corresponds to an unsigned 32 bit integer type.
  The R, G, B and A corresponds respectively to the red, green, blue and alpha channel and each takes 8 bit.
  The alpha channel controls the transparency and its values corresponds to zero for completely transparent and 255, ``0xff`` in hexadecimal, for completely opaque.
  The color can be given directly in hexadecimal format like, for example, ``0x00ff00ff`` for green.

.. c:value:: color_red
.. c:value:: color_blue
.. c:value:: color_green
.. c:value:: color_yellow
.. c:value:: color_black
.. c:value:: color_gray
.. c:value:: color_white
.. c:value:: color_none

Drawing Properties
------------------

The following are used when adding an element into a plot to specify if should be stroked, filled or possibly both.
In addition some properties affect the way the object will be drawn on the screen.
The values are meant tb be combined using logical bit operators.

.. c::value:: canvas_fill

  Draw each closed polygon with a filling color.

.. c::value:: canvas_stroke

  Draw the stroking of the path or contour.

.. c::value:: canvas_outline

  Only the outline of the path will be drawn, without anti-aliasing.

.. c::value:: canvas_crisp

.. c:types:: canvas_object

  A graphical element that can be added to a plot. An instance of the :c:type:`canvas_object` cannot be directly created but it is the base class of other elements like, for example, a :c:type:`canvas_path`.

.. c:type:: canvas_path

  A graphical path that can contain polygonal paths, open or closed. Can be cast into an object using the function :c:func:`path_as_object`.

.. c:function:: canvas_path *canvas_path_new()

  Create a new empty path.

.. c:function:: void canvas_path_move_to(canvas_path *path, double x, double y)

  Move the path to the given location without a connecting line.

.. c:function:: void canvas_path_line_to(canvas_path *path, double x, double y)

  Add a straight line segment up to the given location.

.. c:function:: void canvas_path_close_polygon(canvas_path *path)

  Close the current polygon.

.. c:function:: canvas_object *path_as_object(canvas_path *path)

   This function is used to cast a pointer to canvas_path into a pointer to canvas_object in a type-safe way.

.. cpp:class:: CurvePath : public canvas_object

  A graphical path similar to the :cpp:class:`canvas_path` but can additionally contain Bézier and elliptical arcs. More details about how exactly the arcs are drawn can be found from the `official SVG documentation on Paths <https://svgwg.org/specs/paths/#PathElement>`_.

  Please note that the :cpp:class:`CurvePath` class does not inherit from :cpp:class:`canvas_path` even if it implements all the methods of the latter.
  This is due to the their internal representation.

  .. cpp:function:: CurvePath()

    Create an empty path that can contain Bézier and elliptic arcs.

  .. cpp:function:: void MoveTo(double x, double y)

    Move the path to the given location without a connecting line.

  .. cpp:function:: void LineTo(double x, double y)

    Add a straight line segment up to the given location.

  .. cpp:function:: void Curve3(double x_ctrl, double y_ctrl, double x_to, double y_to)

    Add a quadratic Bézier curve up to the point ``(x_to, y_to)`` with control point ``(x_ctrl, y_ctrl)``.

  .. cpp:function:: void Curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to)

    Add a cubic Bézier curve up to the point ``(x_to, y_to)`` with control points ``(x_ctrl1, y_ctrl1)`` and  ``(x_ctrl2, y_ctrl2)``.

  .. cpp:function:: void ArcTo(double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y)

     Add an elliptical path up to the point  ``(x, y)``. The other parameters are:

     -  ``(rx, ry)``, x and y radius of the ellipse.
     -  ``angle``, rotation angle of the ellipse's x axis.
     -  ``large_arc_flag``, if a large angle or small angle arc should be chosen.
     -  ``bool sweep_flag``, if the arc should be drawn in the sense of increasing angles.

  .. cpp:function:: void ClosePolygon()

    Close the current polygon.

.. cpp:class:: Markers : public canvas_path

  Like a path but instead of drawing a path it draws a marker at each vertex of the path.

  .. cpp:function:: Markers(double size, canvas_object marker_symbol)

    Create an empty Markers path that will draw markers of the specified `size`, in pixels, and using an object `marker_symbol` as a marker. The marker symbol should be an object that span a rectangular area of unit side and centered on zero.

.. cpp:function:: canvas_object MarkerSymbol(int index)

  Returns a marker symbol from a standard list of symbol.

.. cpp:class:: Text : public canvas_object

  An object that draws a text label. It is meant to be drawn with a fill color, without stroking.

  .. cpp:function:: Text(const char* text, double size = 10.0, double hjustif = 0.0, double vjustif = 0.0)

    Create a text object with the given text. The size corresponds to the text size in pixel. The other parameters represents the horizontal and vertical justification. For the horizontal 0.0 corresponds to left justification and 1.0 to right justification. For the vertical 0.0 corresponds to alignment on the text's bottom line and 1.0 to the text's top line.

  .. cpp:function:: void SetAngle(double angle)

    Set the angle, in radians, for the text orientation.

  .. cpp:function:: void SetPosition(double x, double y)

    Set the position of the text.

.. cpp:class:: DashPath : public canvas_path

  A path object but it will be drawn as a dashed lines.

  .. cpp:function:: DashPath(std::initializer_list<double> lst)

    Create a dashed path with the length of the dash and the gap given by the initializer list.
    The given lengths will be used for dashing in the screen coordinates space.
    The number of elements of the initializer list should be a multiple of two so that the elements represents consecutively the length of the dash and the following gap.

  .. cpp:function:: void AddDash(double a, double b)

    Add a dash length and gap after the creation of the object.
