Plot - C API
============

Plot options
------------

  .. c:macro:: canvas_plot_show_units
  .. c:macro:: canvas_plot_auto_limits

Legend Placement
----------------

  .. c:macro:: canvas_plot_left
  .. c:macro:: canvas_plot_right
  .. c:macro:: canvas_plot_bottom
  .. c:macro:: canvas_plot_top

.. c:type:: canvas_plot

  A plot is an object that is used to group and display one or more graphical elements. The elements that belong to a plot will share the same coordinates space, we will refer to this latter as the model coordinates space.

  The plot works by displaying on the screen a region of the model coordinates space.
  When in :c:macro:`canvas_plot_auto_limits` the size of the region will be automatically adjusted to accommodate all the objects contained in the plot. Otherwise the size of the region should be explicitly given using the Plot's method :c:func:`canvas_plot_set_limits`.

  When shown on the screen the plot will show all the objects it contains using for each of them a combination of stroking and filling.
  The plot will also display the X and Y axis with units and labels if the option :c:macro:`canvas_plot_show_units` is used.

  The elements that belongs to a plot are organized in incremental *layers* of visualization that contains one or more graphical elements.
  Layers and be added or removed from a plot in an incremental fashion.

  Examples of a plot that contains two Path objects, drawn with different colors.

  .. figure:: plot-function-example.png

  .. c:function:: canvas_plot *canvas_plot_new(unsigned int flags)

    Create a new empty plot with options given by ``flags``.

  .. c:function:: void canvas_plot_free(canvas_plot *plot)

    Free the plot.

  .. c:function:: void canvas_plot_set_title(canvas_plot *plot, const char *title)

    Set the plot's title.

  .. c:function:: void canvas_plot_set_x_axis_title(canvas_plot *plot, const char *title)

    Set the X axis title.

  .. c:function:: void canvas_plot_set_y_axis_title(canvas_plot *plot, const char *title)

    Set the Y axis title.

  .. c:function:: void canvas_plot_set_clip_mode(canvas_plot *plot, bool flag)

    Set the clip mode. If true all the pixel drawn for the graphics elements are clipped in the plotting area.

  .. c:function:: void canvas_plot_set_limits(canvas_plot *plot, float x0, float y0, float x1, float y1)

    Explicitly set the plotting limits, along the two axis. Attention if the plot is in auto limits the limits will be changed when adding new objects.

  .. c:function:: void canvas_plot_set_label_angle(canvas_plot *plot, int axis, float angle)

    Set the angle in radians for the labels of the X axis.

  .. c:function:: void canvas_plot_enable_label_format(canvas_plot *plot, int axis, const char *fmt)

    Set the format to be used to write the label for the given axis. Follow the same format of the printf function but accepts only specifiers for integer and floating point numbers.

  .. c:function:: void canvas_plot_commit_pending_draw(canvas_plot *plot_object)

    Mark all pending changes as done and they are added to the plot elements.
    **Should not be used** as plots includes now a mechanism to perform this operation automatically.

  .. c:function:: void canvas_plot_add(canvas_plot *plot, canvas_object *obj, canvas_color stroke_color, float stroke_width, canvas_color fill_color, int flags)

    Add a graphical object to the plot by specifying the stroke color, line's width and fill color.

  .. c:function:: bool canvas_plot_push_layer(canvas_plot *plot)

    Create a new empty layer.

  .. c:function:: bool canvas_plot_pop_layer(canvas_plot *plot)

    Discard the current layer with all its content. The previous layer will become the current one. If the plot has only one layer the operation has no effect.

  .. c:function:: void canvas_plot_add_legend(canavs_plot *plot, canavs_plot *legend, int legend_location)

    Add a new plot as a legend in the given location.

  .. c:function:: int canvas_plot_write_svg(canvas_plot *plot, const char *filename, double width, double height)

    Write the plot as a SVG file with the given filename, width and height.
