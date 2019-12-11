
Graphical Elements - C API
==========================

.. c:type:: elp_color 

  Used to express colors in RGBA format. Corresponds to an unsigned 32 bit integer type.
  The R, G, B and A corresponds respectively to the red, green, blue and alpha channel and each takes 8 bit.
  The alpha channel controls the transparency and its values corresponds to zero for completely transparent and 255, ``0xff`` in hexadecimal, for completely opaque.
  The color can be given directly in hexadecimal format like, for example, ``0x00ff00ff`` for green.

Color constants
---------------

The library provides a few color constants listed below.

.. c:macro:: color_red
.. c:macro:: color_blue
.. c:macro:: color_green
.. c:macro:: color_yellow
.. c:macro:: color_black
.. c:macro:: color_gray
.. c:macro:: color_white
.. c:macro:: color_none

Drawing Properties
------------------

The following are used when adding an element into a plot to specify if should be stroked, filled or possibly both.
In addition some properties affect the way the object will be drawn on the screen.
The values are meant tb be combined using logical bit operators.

.. c:macro:: elp_fill

  Draw each closed polygon with a filling color.

.. c:macro:: elp_stroke

  Draw the stroking of the path or contour.

.. c:macro:: elp_outline

  Only the outline of the path will be drawn, without anti-aliasing.

.. c:macro:: elp_crisp

Graphics objects types and functions
------------------------------------

.. c:type:: elp_object

  A graphical element that can be added to a plot. An instance of the :c:type:`elp_object` cannot be directly created but it is the base class of other elements like, for example, a :c:type:`elp_path`.

.. c:type:: elp_path

  A graphical path that can contain polygonal paths, open or closed. Can be cast into an object using the function :c:func:`path_as_object`.

  .. c:function:: elp_path *elp_path_new()

    Create a new empty path.

  .. c:function:: void elp_path_move_to(elp_path *path, double x, double y)

    Move the path to the given location without a connecting line.

  .. c:function:: void elp_path_line_to(elp_path *path, double x, double y)

    Add a straight line segment up to the given location.

  .. c:function:: void elp_path_close_polygon(elp_path *path)

    Close the current polygon.

.. c:function:: elp_object *path_as_object(elp_path *path)

   This function is used to cast a pointer to elp_path into a pointer to elp_object in a type-safe way.

.. c:type:: elp_curve

  A graphical path similar to the :c:type:`elp_path` but can additionally contain Bézier and elliptical arcs. More details about how exactly the arcs are drawn can be found from the `official SVG documentation on Paths <https://svgwg.org/specs/paths/#PathElement>`_.

  Please note that the :c:type:`elp_curve` class does not inherit from :c:type:`elp_path` even if it implements all the methods of the latter.
  This is due to the their internal representation.

  .. c:function:: elp_curve *elp_curve_new()

    Create an new empty path that can contain Bézier and elliptic arcs.

  .. c:function:: elp_curve *elp_curve_copy(const elp_curve *source)

    Create a copy of the :c:type:`elp_curve` object.

  .. c:function:: void elp_curve_move_to(elp_curve *curve, double x, double y)

    Move the path to the given location without a connecting line.

  .. c:function:: void elp_curve_line_to(elp_curve *curve, double x, double y)

    Add a straight line segment up to the given location.

  .. c:function:: void elp_curve_curve3(elp_curve *curve, double x_ctrl, double y_ctrl, double x_to, double y_to)

    Add a quadratic Bézier curve up to the point ``(x_to, y_to)`` with control point ``(x_ctrl, y_ctrl)``.

  .. c:function:: void elp_curve_curve4(elp_curve *curve, double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to)

    Add a cubic Bézier curve up to the point ``(x_to, y_to)`` with control points ``(x_ctrl1, y_ctrl1)`` and  ``(x_ctrl2, y_ctrl2)``.

  .. c:function:: elp_curve_arc_to(elp_curve *curve, double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y)

     Add an elliptical path up to the point  ``(x, y)``. The other parameters are:

     -  ``(rx, ry)``, x and y radius of the ellipse.
     -  ``angle``, rotation angle of the ellipse's x axis.
     -  ``large_arc_flag``, if a large angle or small angle arc should be chosen.
     -  ``bool sweep_flag``, if the arc should be drawn in the sense of increasing angles.

  .. c:function:: void elp_curve_close_polygon()

    Close the current polygon.

.. c:type:: elp_markers

  Like a path but instead of drawing a path it draws a marker at each vertex of the path.

  .. c:function:: elp_markers *elp_markers_new(double size, elp_object marker_symbol)

    Create an empty Markers path that will draw markers of the specified `size`, in pixels, and using an object `marker_symbol` as a marker. The marker symbol should be an object that span a rectangular area of unit side and centered on zero.

  .. c:function:: elp_markers *elp_markers_copy(const elp_markers *markers)

    Create a copy of an object of type :c:type:`canavs_markers`.

  .. c:function:: void elp_markers_free(anvas_markers *markers)

    Free an object of type c:type:`canavs_markers`.

.. c:function:: elp_object *elp_marker_symbol_by_index(int index)

  Returns a marker symbol from a standard list of symbol.
  The symbol is selected by the integer argument.

.. c:function:: elp_object *elp_marker_symbol_by_name(const char *name)

  Returns a marker symbol from a standard list of symbol.
  The symbol is selected by name.

.. cpp:class:: Text : public elp_object

  An object that draws a text label. It is meant to be drawn with a fill color, without stroking.

  .. cpp:function:: Text(const char* text, double size = 10.0, double hjustif = 0.0, double vjustif = 0.0)

    Create a text object with the given text. The size corresponds to the text size in pixel. The other parameters represents the horizontal and vertical justification. For the horizontal 0.0 corresponds to left justification and 1.0 to right justification. For the vertical 0.0 corresponds to alignment on the text's bottom line and 1.0 to the text's top line.

  .. cpp:function:: void SetAngle(double angle)

    Set the angle, in radians, for the text orientation.

  .. cpp:function:: void SetPosition(double x, double y)

    Set the position of the text.

.. cpp:class:: DashPath : public elp_path

  A path object but it will be drawn as a dashed lines.

  .. cpp:function:: DashPath(std::initializer_list<double> lst)

    Create a dashed path with the length of the dash and the gap given by the initializer list.
    The given lengths will be used for dashing in the screen coordinates space.
    The number of elements of the initializer list should be a multiple of two so that the elements represents consecutively the length of the dash and the following gap.

  .. cpp:function:: void AddDash(double a, double b)

    Add a dash length and gap after the creation of the object.
