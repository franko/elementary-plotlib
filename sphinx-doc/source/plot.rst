Plot
====

.. cpp:namespace:: libcanvas

.. cpp:class:: Plot

  A plot that can contains graphical elements organized in layers. The plot has some logical limits that corresponds to the the coordinates of the graphical elements. The limits can be explicitly given or they can be automatically determinate by the plot to accomodate all the objects it contains.

  The plot can also draw the plot's axis with the labels and it can contains one or more plot legends that can be placed outside of the plot's area.

  Examples of a plot that contains two Path objects, drawn with different colors.

  .. figure:: plot-function-example.png

  Plot options:
  ~~~~~~~~~~~~~~

  .. cpp:enumerator:: ShowUnits
  .. cpp:enumerator:: AutoLimits

  Legend Placement
  ~~~~~~~~~~~~~~~~

  .. cpp:enumerator:: Left
  .. cpp:enumerator:: Right
  .. cpp:enumerator:: Bottom
  .. cpp:enumerator:: Top

  .. cpp:function:: Plot(unsigned flags = 0)

    Create a new empty plot with options given by ``flags``.

  .. cpp:function:: void SetTitle(const char *title)

    Set the plot's title.

  .. cpp:function:: void SetXAxisTitle(const char *axis_title)

    Set the X axis' title.

  .. cpp:function:: void SetClipMode(bool flag)

    Set the clip mode. If true all the pixel drawn for the graphics elements are clipped in the plotting area.

  .. cpp:function:: void SetLimits(const Rectangle& r)

    Explicitly set the plotting limits, along the two axis. Attention if the plot is in auto limits the limits will be changed when adding new objects.

  .. cpp:function:: void SetAxisLabelsAngle(const Axis& axis, float angle)

    Set the angle in radians for the labels of the X axis.

  .. cpp:function:: void EnableLabelFormat(const Axis& axis, const char *fmt)

    Set the format to be used to write the label for the given axis. Follow the same format of the printf function nut only the most common options for numbers are supported.

  .. cpp:function:: void CommitPendingDraw()

    Mark all pending changes as done and they are added to the plot elements.

  .. cpp:function:: void Add(Object object, Color stroke_color, float stroke_width, Color fill_color, unsigned flags = property::Fill | property::Stroke)

    Add a graphical object to the plot by specifying the stroke color, line's width and fill color.

  .. cpp:function:: bool PushLayer()

    Create a new empty layer.

  .. cpp:function:: bool PopLayer()

    Discard the current layer with all its content. The previous layer will become the current one. If the plot has only one layer the operation has no effect.

  .. cpp:function:: void AddLegend(Plot legend, Placement location)

    Add a new plot as a legend in the given location.
