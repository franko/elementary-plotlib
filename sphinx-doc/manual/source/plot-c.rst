Plot - C API
============

Plot options
------------

  .. c:macro:: elem_plot_show_units
  .. c:macro:: elem_plot_auto_limits
  .. c:macro:: elem_plot_clip_region

Legend Placement
----------------

  .. c:macro:: elem_plot_left
  .. c:macro:: elem_plot_right
  .. c:macro:: elem_plot_bottom
  .. c:macro:: elem_plot_top

.. c:type:: elem_plot

  A plot is an object that is used to group and display one or more graphical elements. The elements that belong to a plot will share the same coordinates space, we will refer to this latter as the model coordinates space.

  The plot works by displaying on the screen a region of the model coordinates space.
  When in :c:macro:`elem_plot_auto_limits` the size of the region will be automatically adjusted to accommodate all the objects contained in the plot. Otherwise the size of the region should be explicitly given using the Plot's method :c:func:`elem_plot_set_limits`.

  When shown on the screen the plot will show all the objects it contains using for each of them a combination of stroking and filling.
  The plot will also display the X and Y axis with units and labels if the option :c:macro:`elem_plot_show_units` is used.

  The elements that belongs to a plot are organized in incremental *layers* of visualization that contains one or more graphical elements.
  Layers and be added or removed from a plot in an incremental fashion.

  Examples of a plot that contains two Path objects, drawn with different colors.

  .. figure:: plot-function-example.png

  .. c:function:: elem_plot *elem_plot_new(unsigned int flags)

    Create a new empty plot with options given by ``flags``.

  .. c:function:: void elem_plot_free(elem_plot *plot)

    Free the plot.

  .. c:function:: void elem_plot_set_title(elem_plot *plot, const char *title)

    Set the plot's title.

  .. c:function:: void elem_plot_set_x_axis_title(elem_plot *plot, const char *title)

    Set the X axis title.

  .. c:function:: void elem_plot_set_y_axis_title(elem_plot *plot, const char *title)

    Set the Y axis title.

  .. c:function:: void elem_plot_set_clip_mode(elem_plot *plot, bool flag)

    Set the clip mode. If true all the pixel drawn for the graphics elements are clipped in the plotting area.

  .. c:function:: void elem_plot_set_limits(elem_plot *plot, float x0, float y0, float x1, float y1)

    Explicitly set the plotting limits, along the two axis. Attention if the plot is in auto limits the limits will be changed when adding new objects.

  .. c:function:: void elem_plot_set_label_angle(elem_plot *plot, int axis, float angle)

    Set the angle in radians for the labels of the X axis.

  .. c:function:: void elem_plot_enable_label_format(elem_plot *plot, int axis, const char *fmt)

    Set the format to be used to write the label for the given axis. Follow the same format of the printf function but accepts only specifiers for integer and floating point numbers.

  .. c:function:: void elem_plot_commit_pending_draw(elem_plot *plot_object)

    Mark all pending changes as done and they are added to the plot elements.
    **Should not be used** as plots includes now a mechanism to perform this operation automatically.

  .. c:function:: void elem_plot_add(elem_plot *plot, elem_object *obj, elem_color stroke_color, float stroke_width, elem_color fill_color, int flags)

    Add a graphical object to the plot by specifying the stroke color, line's width and fill color.

  .. c:function:: bool elem_plot_push_layer(elem_plot *plot)

    Create a new empty layer.

  .. c:function:: bool elem_plot_pop_layer(elem_plot *plot)

    Discard the current layer with all its content. The previous layer will become the current one. If the plot has only one layer the operation has no effect.

  .. c:function:: void elem_plot_add_legend(canavs_plot *plot, canavs_plot *legend, int legend_location)

    Add a new plot as a legend in the given location.

  .. c:function:: void elem_plot_show(elem_plot *plot, unsigned width, unsigned height, unsigned flags)

    Show the plot on the screen with the given width and height.
    The flags argument is currently unused and should be set to :c:macro:`elem_window_resize`.
    It works internally by creating and showing on the screen a window.
    The function :c:func:`elem_window_wait` can be used to wait until the window is closed.

  .. c:function:: void elem_plot_wait(elem_plot *plot)

    If the plot is shown on the screen, using the :c:func:`elem_plot_show`, it waits until the window is closed.

  .. c:function:: int elem_plot_write_svg(elem_plot *plot, const char *filename, double width, double height)

    Write the plot as a SVG file with the given filename, width and height.
