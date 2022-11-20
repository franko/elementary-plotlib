Plot
====

.. cpp:namespace:: elem

.. cpp:class:: Plot

  A plot is an object that is used to group and display one or more graphical elements. The elements that belong to a plot will share the same coordinates space, we will refer to this latter as the model coordinates space.

  The plot works by displaying on the screen a region of the model coordinates space.
  When in :cpp:enumerator:`Autolimits` the size of the region will be automatically adjusted to accommodate all the objects contained in the plot. Otherwise the size of the region should be explicitly given using the Plot's method :cpp:func:`Plot::SetLimits`.

  When shown on the screen the plot will show all the objects it contains using for each of them a combination of stroking and filling.
  The plot will also display the X and Y axis with units and labels if the option :cpp:enumerator:`ShowUnits` is used.

  The elements that belongs to a plot are organized in incremental *layers* of visualization that contains one or more graphical elements.
  Layers and be added or removed from a plot in an incremental fashion.

  Examples of a plot that contains two Path objects, drawn with different colors.

  .. figure:: plot-function-example.png

  **Plot options**

  .. cpp:enumerator:: ShowUnits
  .. cpp:enumerator:: AutoLimits
  .. cpp:enumerator:: ClipRegion

  **Legend Placement**

  .. cpp:enumerator:: Left
  .. cpp:enumerator:: Right
  .. cpp:enumerator:: Bottom
  .. cpp:enumerator:: Top

  .. cpp:function:: Plot(unsigned flags = 0)

    Create a new empty plot with options given by ``flags``.

  .. cpp:function:: void SetTitle(const char *title)

    Set the plot's title.

  .. cpp:function:: void SetXAxisTitle(const char *axis_title)

    Set the X axis title.

  .. cpp:function:: void SetYAxisTitle(const char *axis_title)

    Set the Y axis title.

  .. cpp:function:: void SetClipMode(bool flag)

    Set the clip mode. If true all the pixel drawn for the graphics elements are clipped in the plotting area.

  .. cpp:function:: void SetLimits(float x1, float y1, float x2, float y2)

    Explicitly set the plotting limits, along the two axis.
    If the plot is in auto limits mode the limits will be changed when adding new objects.

  .. cpp:function:: void SetAxisLabelsAngle(const Axis& axis, float angle)

    Set the angle in radians for the labels of the X axis.

  .. cpp:function:: void EnableLabelFormat(const Axis& axis, const char *fmt)

    Set the format to be used to write the label for the given axis. Follow the same format of the printf function but accepts only specifiers for integer and floating point numbers.

  .. cpp:function:: void CommitPendingDraw()

    Mark all pending changes as done and they are added to the plot elements.
    **Should not be used** as plots includes now a mechanism to perform this operation automatically.

  .. cpp:function:: void Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill | property::Stroke)

    Add a graphical object to the plot by specifying the stroke color, line's width and fill color.

  .. cpp:function:: void AddStroke(Object object, Color color, float line_width, unsigned flags = property::Stroke)

    Add a graphical object to the plot by specifying the stroke color, line's width but without fill. Equivalent to :cpp:func:`Plot::Add` with fill color set to ``color::None``.

  .. cpp:function:: bool PushLayer()

    Create a new empty layer.

  .. cpp:function:: bool PopLayer()

    Discard the current layer with all its content. The previous layer will become the current one. If the plot has only one layer the operation has no effect.

  .. cpp:function:: void SetLegend(Plot legend, Placement location)

    Set the given plot as a legend in the given location.

  .. cpp:function:: void GetLegend(Placement location)

    Returns a copy of the plot's legend in the given location.

  .. cpp:function:: void Show(unsigned width, unsigned height, unsigned flags)

    Show the plot on the screen with the given width and height.
    The flags argument is currently unused and should be set to :cpp:enumerator:`WindowResize`.
    It works internally by creating and showing on the screen a window.
    The function :cpp:func:`Plot::Wait` can be used to wait until the window is closed.

  .. cpp:function:: void Wait()

    If the plot is shown on the screen, using the :cpp:func:`Plot::Show`, it waits until the window is closed.

  .. cpp:function:: bool WriteSvg(const char *filename, double width, double height)

    Write the plot as a SVG file with the given filename, width and height.


Drawing Properties
------------------

The following are used when adding an element into a plot to specify if should be stroked, filled or possibly both.
In addition some properties affect the way the object will be drawn on the screen.
The values are meant to be combined using logical bit operators.

.. cpp:namespace-push:: property

.. cpp:enumerator:: Fill

  Draw each closed polygon with a filling color.

.. cpp:enumerator:: Stroke

  Draw the stroking of the path or contour.

.. cpp:enumerator:: Outline

  Only the outline of the path will be drawn, without anti-aliasing.

.. cpp:enumerator:: Crisp

.. cpp:namespace-pop::
