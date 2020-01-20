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

.. cpp:struct:: window_fox_start_data

  Contains the following field:
  
  - `window_fox *window`
  - `unsigned width`
  - `unsigned height`
  - `unsigned flags`

.. cpp:function:: elp_window *elp_window_fox(FXApp *app, FXObject *host_object, FXuint start_sel)
  
  Create a new object that can be passed to :cpp:func:`FX::FXElemWindow::Attach` to create a :cpp:class:`elp::Window`.
  It should be used on a worker thread to create a :cpp:class:`elp::Window` that is started in the FOX GUI thread.
  The arguments represent in the order: the FXApp, an FXObject which is responsible to create a FXElemBaseWindow.
  The FXObject `host_object` will receive a signal SEL_IO_READ with id `start_sel`.

  Here an example of how the handler should be done::

    long SomeClass::onElemWindowStart(FXObject *, FXSelector, void *ptr) {
        window_fox_start_data *message_data = (window_fox_start_data *) ptr;
        FXApp *app = getApp();
        FXuint main_window_options = (DECOR_TITLE|DECOR_MINIMIZE|DECOR_MAXIMIZE|DECOR_CLOSE|DECOR_BORDER|DECOR_SHRINKABLE|DECOR_MENU);
        if (message_data->flags & WindowResize) {
            main_window_options |= DECOR_STRETCHABLE;
        }
        auto main_win = new FXMainWindow(app, "Plot Window", nullptr, nullptr, main_window_options, 0, 0, message_data->width, message_data->height);
        auto plot_win = new FXElemBaseWindow(main_win, message_data->window, LAYOUT_FILL_X | LAYOUT_FILL_Y);
        message_data->window->bind_drawable(plot_win, FXElemBaseWindow::ID_UPDATE_REGION);
        main_win->create();
        main_win->show(PLACEMENT_SCREEN);
        return 1;
    }

  The notable things are:

  - a :cpp:class:`window_fox_start_data` message is provided
  - a FXElemBaseWindow is created inside some container
  - bind_drawable is called as in the example above
  - the window is created and shown
