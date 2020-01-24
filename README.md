Elementary Plot Library — *A C/C++ library for high-quality plots and animations*

Looking for an open-source library that gives you a simple API to easily
create high-quality plots on major desktop platforms? Do you need to pop
a native window to display a plot without the need of a third-party library?
Do you need integration with the FOX toolkit?
*Here it is.* And it's free to use, even for commercial purposes.

[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Beauty of simplicity**

Simple C and C++ API to easily create and display plots with a few lines of code. Create animations with simple layer-based functions by adding elements.

**With batteries included**

Display a native window to display the plot or the animations without thind-party libraries on Windows and X Window's based systems.

**High quality plots**

the Elementary Plot library use the Anti-Grain library with subpixel resolution to create high-quality plots. The fonts are rendered using state of the art algorithm for font rendering but only basic text layout is supported.

**Integration with existing toolkits**

The library is thought to be integrated with existing GUI toolkits with minimal work. Currently only an implementation for the [FOX Toolkit](http://www.fox-toolkit.org/) is provided but it can be used as an example to integrate the Elementary Plot library with other toolkits.

**Documentation available**

The [Elementary Plotlib reference manual](http://franko.github.io/elementary-plotlib/manual/) for the new API is available.

Examples
========

Here an example of a plot with a function and some markers.

![Plot of function with markers](https://github.com/franko/elementary-plotlib/blob/master/sphinx-doc/manual/source/plot-function-example.png)

The corresponding source code is available [here](https://github.com/franko/elementary-plotlib/blob/master/tests/test-window-function.cpp).

Features and Usage
---

The utilization of the library revolves around the plot and the window classes. The plot can be used to add graphical elements like lines or more complex objects and the window will just show on the screen the content of the plot.

The plot will take automatically cares of choosing the more appropriate limits, will draw the axis with the labels and in general all the elements needed for a plot. Plots presentation and their behavior can be changed by setting plot options, for example you can choose to don't show the plot's axis.

To make faster animations the graphical elements inside a plot can be organized in *layers*. New layers can be added to group all the newly added objects. A layer can be removed removing all the objects that belongs to the layer.

The window is used to show the plot on the screen. It can accomodate one plots or more by choosing a special layout. When a plot is updated by adding or removing some elements the window on the screen can be updated by calling a specific function.

Installing the library
---

The library can be compiled using the Meson build system and it does require the AGG and freetype2 libraries. If the FOX library is detected in addition the FOX support library will be built.

To build the library and the examples use the following command:
```
> meson build
> ninja -C build
```

Rendering of graphical elements
----

All the graphical elements are represented in vector form. The rendering is done using the Agg library with subpixel resolution or with ordinary anti-alias.

The text drawing is doing using the freetype library. The font system is extremely simple and thought to display only simple ascii-based text.


Current status
---

The Elementary Plot library is quite solid in its core since it was used previously in two other projects.
Its current status is 1.0 pre-release.

A clean and simple C and C++ API is now available to make the utilisation of the library super-simple and to isolate from the implementation details.

The library currently works on Windows and GNU/Linux system.

The documentation for the library is available in the form of a [reference manual](http://franko.github.io/elementary-plotlib/).

If you use the library expect to find bugs because the library was recently over-hauled and it is not largely tested.

Limitations
---

Be aware that the library does not offer as many features as that other more advanced libraries.

Among the more important limitations of the current implementation:

  - no mouse of keyboard events from the windows can be treated
  - no advanced text-layout capabilities
  - only a single type of font is available
  - no harware acceleration
  - plot has only a few options and are generally not very customizable

More Screenshots
---

***The examples below are obsolete with the new API but still somewhat relevant in developer mode***.

Below an example that show a customized pipeline to render a path:

![Plot custom pipeline](https://github.com/franko/elementary-plotlib/blob/master/doc/libcanvas-custom-pipeline.png)

Another example to show different rendering options:

![Plot custom pipeline](https://github.com/franko/elementary-plotlib/blob/master/doc/libcanvas-rendering-options.png).
