Windows
=======

.. cpp:namespace:: elp

Window's flags
--------------

.. cpp:enumerator:: WindowResize

.. cpp:class:: Window

  A window that can accommodate one or more plots to be shown on the screen.

  .. cpp:function:: Window()

  Create an empty window with a single slot.

  .. cpp:function:: Window(const char *layout)

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

  .. cpp:function:: void SetLayout(const char *layout)

    Set the window's layout using the format string `layout` like when using the constructor :cpp:func:`Window::Window()`.
    Can be used only if the windows is not yet running.
    If the window is already running the call has no effect.

  .. cpp:function:: int Attach(Plot& plot, const char* slot)

    Attach the given plot inside a window using ``slot`` to address the slot.
    Return an integer index that can be used for the :cpp:func:`Window::SlotRefresh` function.

    The ``slot`` string should be composed by integer indexes separated by commas.
    Each index is used to address a position inside an horizontal or vertical split.
    Further indexes are used to address a sub-layout, if any is present.

    For example for a window layout specified by ``h..`` the simple strings ``1`` or ``2`` will address respectively the first and the second slot.
    For a layout with nested subdivisions, for example, ``h(v..)(v..)`` the string ``1,2`` will address the slot in the first column and the second row.


  .. cpp:function:: void SlotRefresh(unsigned index)

    Incrementally update the image of the window for the plot in the slot identified by ``index``.
    Only the elements on the plot marked as pending will be drawn.


  .. cpp:function:: void Start(unsigned width, unsigned height, unsigned flags)

    Show the window on the screen with the given width and height.
    Thhe flags currently accept only the :cpp:enumerator:`WindowResize`.

  .. cpp:function:: void Wait()

    Blocks waiting until the window is closed.
