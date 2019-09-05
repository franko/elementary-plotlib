Windows
=======

.. cpp:namespace:: libcanvas

.. cpp:class:: Window

  A window that can accomodate one or more plots to be shown on the screen.

  .. cpp:function:: Window()

  Create an empty window with a single slot.

  .. cpp:function:: Window(const char *fmt)

  Create an empty window subdivided accordingly to the ``fmt`` string.

  The format string works by recognizing the following elements:
  


    int Attach(Plot& plot, const char* slot_str);
    void SlotRefresh(unsigned index);
    void Start(unsigned width, unsigned height, unsigned flags);
    void Wait();
