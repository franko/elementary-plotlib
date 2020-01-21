FOX toolkit integration
=======================

The Elementary Plot library can be used inside a `FOX Toolkit <http://fox-toolkit.org/>`_ application using a special class :cpp:class:`FX::FXElemWindow` declared in the header file ``FXElemWindow.h``.

The :cpp:class:`FX::FXElemWindow` inherit from ``FX::FXElemBaseWindow`` which in turns inherits from ``FX::FXWindow`` and it can be used as an ordinary FOX Window in every respect.
In addition the class implements the method :cpp:func:`FX::FXElemWindow::Attach` to bind a plot, like it can be done with a :cpp:class:`elp::Window`.

The plots themselves can be created and modified using the standard C++ API of the Elementary Plot Library.

Once a plot is bound to a window the application will be automatically updated for any plot's modification, without the need of any explicit action.

.. cpp:class:: FX::FXElemWindow : public FX::FXElemBaseWindow

  FOX toolkit class used to display one or more Elementary Plot plots.

  .. cpp:function:: FX::FXElemWindow(FX::FXComposite* p, const char *plot_layout = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0)

    Ordinary FOX constructor with the addition of ``plot_layout`` used to subdivide the windows in multiple plotting slots.

  .. cpp:function:: int Attach(elp::Plot& p, const char* slot_str)

    Act like the method :cpp:class:`elp::Window` of the :cpp:class:`elp::Window` class to bind a plot to a given slot.

  .. cpp:function:: void SlotRefresh(unsigned index)

    Act like the :cpp:class:`elp::Window`'s method of the same name.

  .. cpp:function:: void SetLayout(const char *fmt)

    Change the plot's layout of the window.
    It works only if the window is not yet started.

  .. cpp:function:: void Wait()

    Act like the :cpp:class:`elp::Window`'s method of the same name.
    Normally not needed in classic FOX applications.

.. cpp:class:: FX::FXElemBaseWindow : public FX::FXWindow

  FOX toolkit class that provide support for and :cpp:class:`elp::Window`.
  It is used for the implementation and should not be used directly.

.. cpp:struct:: FXElemStartMessage

  Contains the following field:
  
  - `window_fox *window`
  - `unsigned width`
  - `unsigned height`
  - `unsigned flags`

.. cpp:function:: elp_window *elp_window_fox(FXGUISignal *start_signal)
  
  Create a new object that can be passed to :cpp:func:`FX::FXElemWindow::Attach` to create a :cpp:class:`elp::Window`.
  It should be used on a worker thread to create a :cpp:class:`elp::Window` that is started in the FOX GUI thread.
  The start_signal should be a signal, previously created in the GUI thread, that trigger a request to create a new Elementary Plot window.
  The method that handle the start_signal should call the function :cpp:func:`FXElemBuildWindow` passing a FXComposite where the Elementary Plot window should be placed.

  Here an example of how the handler cound be done::

    long SomeClass::onElemWindowStart(FXObject *, FXSelector, void *ptr) {
      FXElemStartMessage *message = (FXElemStartMessage *) ptr;
      assert(message != nullptr);
      FXuint main_window_options = (DECOR_TITLE|DECOR_MINIMIZE|DECOR_MAXIMIZE|DECOR_CLOSE|DECOR_BORDER|DECOR_SHRINKABLE|DECOR_MENU);
      if (message->flags & WindowResize) {
          main_window_options |= DECOR_STRETCHABLE;
      }
      FXMainWindow *main_win = new FXMainWindow(getApp(), "Plot Window", nullptr, nullptr, main_window_options, 0, 0, message->width, message->height);
      FXElemBuildWindow(main_win, message, ELEM_CREATE_DEFER);
      main_win->create();
      main_win->show(PLACEMENT_SCREEN);
      return 1;
    }

  The notable things are:

  - a :cpp:class:`FXElemStartMessage` message is provided to the handler
  - the function FXElemBuildWindow is called provide a parent composite window
  - the window is created and shown

  Otherwise, if the Elementary Plot window should be created in an already existing FXComposite parent the handler could be::

    long PlotWindow::onElemWindowStart(FXObject *, FXSelector, void *ptr) {
        FXElemStartMessage *message = (FXElemStartMessage *) ptr;
        assert(message != nullptr);
        FXElemBuildWindow(this->frame, message, ELEM_CREATE_NOW);
        return 1;
    }
