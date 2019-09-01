Libcanvas library's documentation
=================================

class Object
------------

A graphical shape that can be added to a plot. An instance of the `class Object`_ cannot be directly created but it is
the base class of other elements like, for example, a `class Path`_.

class Path
----------

A graphical path that can polygonal paths, open or closed.

.. _Path contructor:
``Path()``
  Create a new empty path.

``void MoveTo(double x, double y)``
   Move the path to the given location.

``void LineTo(double x, double y)``
   Add a straight line segment up to the given location.

class CurvePath
---------------

A graphical path that can contain bezier curves and ellipse arcs. It behaves otherwise like an ordinary `class Path`_.

``CurvePath()``
  Create a new empty curve.

``void MoveTo(double x, double y)``
   Move the path to the given location.

``void LineTo(double x, double y)``
   Add a straight line segment up to the given location.

``void Curve3(double x_ctrl, double y_ctrl, double x_to, double y_to)``
   Add a quadratic Bezier arc up to the point ``(x_to, y_to)`` with control point ``(x_ctrl, y_ctrl)``.

``void Curve4(double x_ctrl1, double y_ctrl1, double x_ctrl2, double y_ctrl2, double x_to, double y_to)``
   Add a cubic Bezier arc up to the point ``(x_to, y_to)`` with control points ``(x_ctrl1, y_ctrl1)`` and ``(x_ctrl2, y_ctrl2)``.

``void ArcTo(double rx, double ry, double angle, bool large_arc_flag, bool sweep_flag, double x, double y)``
   Add an elliptic arc up to the point ``(x, y)``. The parameter ``rx`` and ``ry`` corresponds to the x and y radius of the ellipse. ``angle`` represent the rotation in degrees of the ellipse. The ``large_arc`` and ``sweep`` flag allows to chose which arc must be drawn as 4 possible arcs can be drawn out of the other parameters.

``void ClosePolygon()``
   Close the current polygon.

=====  =====  ====== 
   Inputs     Output 
------------  ------ 
  A      B    A or B 
=====  =====  ====== 
False  False  False 
True   False  True 
False  True   True 
True   True   True 
=====  =====  ======
