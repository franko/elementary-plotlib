.. Elementary Plot Library's Homepage documentation master file, created by
   sphinx-quickstart on Sun Jan 19 21:59:55 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Elementary Plot Library's Homepage
=============================================

Elementary Plot Library is a C/C++ library for high-quality plots with a clean and simple C and C++ interface. In addition it provides also support for simple animations.

The library can be used create plots and show them on the screen directly from C or C++ code.
It works by adding graphical elements in plots and by attaching the plots to windows to show them on the screen.
By default each plot automatically handles the scale and show the plotting axis and labels but the behavior of the plot is customizable.

The library use by default the native windows provided by the hosting system.
In addition it can be integrated with existing GUI toolkits but currently supports only the FOX toolkit.

The library is supported on Windows, using MinGW and on Linux.
It has minimal dependency by requiring only the AGG and freetype2 libraries.

Documentation
-------------

The Elementary Plot Library `user manual <https://franko.github.io/elementary-plotlib/manual/>`_.

TODO: add information about how to build the software

TODO: link to github page

A Simple Example
----------------

.. highlight: c++

Here an example::

    #include <cmath>
    #include "libelplot.h"

    using namespace elp;

    int main() {
        InitializeFonts();

        Plot plot;
        const double x0 = 0.0001, x1 = 8 * math::Tau();
        plot.AddStroke(FxLine(x0, x1, [](double x) { return std::sin(x) / x; }), color::Blue, 1.5);

        plot.SetTitle("Function plot example");
        plot.SetXAxisTitle("x variable");
        plot.SetYAxisTitle("y variable");

        Window window;
        window.Attach(plot, "");
        window.Start(640, 480, WindowResize);

        plot.AddStroke(FxLine(0.8, x1, [](double x) { return std::cos(x) / x; }), color::Red, 1.5);
        window.Wait();
        return 0;
    }

In the code above we:

- initialize the library
- create a plot
- add a first line, created with FxLine, to the plot using the blue color
- set the plot's title and its axis titles as well
- create a window
- attach the plot to the window
- show the window
- add a second line to the plot using the red color. The window will be automatically updated.
- wait until the window is closed

Here how the plot looks like:

.. figure:: plot-function-example.png

.. toctree::
   :maxdepth: 2
   :caption: Contents:

.. Not used
   Indices and tables
   ==================

   * :ref:`genindex`
   * :ref:`modindex`
   * :ref:`search`
