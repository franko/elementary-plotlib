Windows - C API
===============

Window's flags
--------------

.. c:macro:: elem_window_resize

.. c:type:: elem_window

  A window that can accommodate one or more plots to be shown on the screen.

  .. c:function:: elem_window *elem_window_new()

  Create an empty window with a single slot.

  .. c:function:: elem_window *elem_window_new_with_layout(const char *fmt)

  Create an empty window subdivided accordingly to the ``layout`` string.

  The format string describe the layout of plots inside the window using the following grammar:

  .. code-block::

    layout = element | h | v ;
    h = "h" { layout } ;
    v = "v" { layout } ;
    element = "." | "(" layout ")" ;

  The character ``.`` means a slot to accommodate a single plot.
  The letters ``h`` and ``v`` indicate respectively an horizontal and a vertical split and are followed by one or more sub-layouts.
  The parenthesis are used fro grouping.

  For example to accommodate two plots horizontally use the layout string ``h..``.
  To subdivide a window in a 2x2 grid it can be used the layout string ``h(v..)(v..)``.

  .. c:function:: void elem_window_set_layout(elem_window *win, const char *layout)

    Set the window's layout using the format string `layout` like when using the constructor :c:func:`elem_window_new_with_layout(const char *layout)`.
    Can be used only if the windows is not yet running.
    If the window is already running the call has no effect.

  .. c:function:: int elem_window_attach(elem_window *win, elem_plot *plot, const char* slot_str)

    Attach the given plot inside a window using ``slot`` to address the slot.
    Return an integer index that can be used for the :cpp:func:`Window::SlotRefresh` function.

    The ``slot`` string should be composed by integer indexes separated by commas.
    Each index is used to address a position inside an horizontal or vertical split.
    Further indexes are used to address a sub-layout, if any is present.

    For example for a window layout specified by ``h..`` the simple strings ``1`` or ``2`` will address respectively the first and the second slot.
    For a layout with nested subdivisions, for example, ``h(v..)(v..)`` the string ``1,2`` will address the slot in the first column and the second row.


  .. c:function:: void elem_window_slot_refresh(elem_window *win, unsigned index)

    Incrementally update the image of the window for the plot in the slot identified by ``index``.
    Only the elements on the plot marked as pending will be drawn.


  .. c:function:: void elem_window_start(elem_window *win, unsigned width, unsigned height, unsigned flags)

    Show the window on the screen with the given width and height.
    Thhe flags currently accept only the :cpp:enumerator:`WindowResize`.

  .. c:function:: void elem_window_wait(elem_window *win)

    Blocks waiting until the window is closed.
