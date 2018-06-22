Libcanvas — *A C++ library for high-quality plots and animations*

Looking for an open-source library that gives you a simple API to easily
create high-quality plots on major desktop platforms? Do you need to pop
a native window to display a plot without the need of a third-party library?
Do you need integration with an existing toolkit?
*Here it is.* And it's free to use, even for commercial purposes.

[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Beauty of simplicity
====================

Simple C++ API to easily create and display plots with a few lines of code. Create animations with simple layer-based functions by adding elements.

With batteries included
=======================

Display a native window to display the plot or the animations without thind-party libraries on Windows and X Window's based systems.

High quality plots
==================

Libcanvas use the Anti-Grain library with subpixel resolution to create high-quality plots. The fonts are rendered using state of the art algorithm for font rendering but only basic text layout is supported.

Integration with existing toolkits
==================================

The library is thought to be integrated with existing GUI toolkits with minimal work. Currently only an implementation for the [FOX Toolkit](http://www.fox-toolkit.org/) is provided but it can be used as an example to integrate libcanvas with other toolkits.

Examples
========

Here an example of a plot with a function and some markers.

![Plot of function with markers](https://github.com/franko/libcanvas/blob/master/doc/libcanvas-function-markers.png)

The corresponding source code is available [here](https://github.com/franko/libcanvas/blob/master/tests/test-window-markers.cpp).

Features and Usage
---

The utilization of the library revolves around the plot and the window classes. The plot can be used to add graphical elements like lines or more complex objects and the window will just show on the screen the content of the plot.

The plot will take automatically cares of choosing the more appropriate limits, will draw the axis with the labels and in general all the elements needed for a plot. Plots presentation and their behavior can be changed by setting plot options, for example you can choose to don't show the plot's axis.

To make faster animations the graphical elements inside a plot can be organized in *layers*. New layers can be added to group all the newly added objects. A layer can be removed removing all the objects that belongs to the layer.

The window is used to show the plot on the screen. It can accomodate one plots or more by choosing a special layout. When a plot is updated by adding or removing some elements the window on the screen can be updated by calling a specific function.

Installing the library
---

The library can be compiled using the Meson build system and it does requires the AGG and freetype2 libraries. Currently the Meson configuration required the FOX library but it is only used for the FOX toolkit integration and can be disabled if needed.

To build the library and the examples use the following command:
```
> meson --buildtype=debug build-debug
> cd build-debug
> ninja # or make
```
As an alternative to Meson there is also a basic support for CMake but it is not currently updated and is not recommended.

Rendering of graphical elements
----

All the graphical elements are represented in vector form. The rendering is done using the Agg library with subpixel resolution or with ordinary anti-alias.

The text drawing is doing using the freetype library. The font system is extremely simple and thought to display only simple ascii-based text.


Current status
---

The libcanvas library is quite solid  in its core since it was used previously in two other projects. Nevertheless in its current incarnation it is undergoing a major rework to simplify and improve its C++ implementation and API.

It currently works on Windows and GNU/Linux system but there will be major changes in the API interface. For example the name of the library and the namespace it uses are temporary and will probably change.

For the moment there is no documentation for the library, only some examples are provided.

Expect also to find bugs especially since the library is currently being reworked.

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

Below an example that show a customized pipeline to render a path:

![Plot custom pipeline](https://github.com/franko/libcanvas/blob/master/doc/libcanvas-custom-pipeline.png)

Another example to show different rendering options:

![Plot custom pipeline](https://github.com/franko/libcanvas/blob/master/doc/libcanvas-rendering-options.png).
