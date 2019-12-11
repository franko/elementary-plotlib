FOX toolkit integration
=======================

The Libcanvas library can be used inside a `FOX Toolkit <http://fox-toolkit.org/>`_ application using a special class :cpp:class:`FX::FXElpWindow` declared in the header file ``FXElpWindow.h``.

The :cpp:class:`FX::FXElpWindow` inherit from ``FX::FXWindow`` and can be used as an ordinary FOX Window in every respect.
In addition the class implements the method :cpp:func:`FX::FXElpWindow::Attach` to bind a plot, like it can be done with a :cpp:class:`libcanvas::Window`.

The plots themselves can be created and modified using the standard Libcanvas functions.

Once a plot is bound to a window the application will be automatically updated for any plot's modification, without the need of any explicit action.

.. cpp:class:: FX::FXElpWindow : public FX::FXWindow

  FOX toolkit class used to display one or more Libcanvas plots.

  .. cpp:function:: FX::FXElpWindow(FX::FXComposite* p, const char *plot_layout = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0)

    Ordinary FOX constructor with the addition of ``plot_layout`` used to subdivide the windows in multiple plotting slots.

  .. cpp:function:: int Attach(libcanvas::Plot& p, const char* slot_str)

    Act like the method :cpp:class:`libcanvas::Window` of the :cpp:class:`libcanvas::Window` class to bind a plot to a given slot.

  .. cpp:function:: void SlotRefresh(unsigned index)

    Act like the :cpp:class:`libcanvas::Window`'s method of the same name.

  .. cpp:function:: void Wait()

    Act like the :cpp:class:`libcanvas::Window`'s method of the same name.
    Normally not needed in classic FOX applications.

