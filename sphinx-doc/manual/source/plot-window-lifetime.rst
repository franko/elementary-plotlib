Plots and Windows lifetime
==========================

The Elementary Plot library provide a special method to ensure that plot and windows are not prematurely disposed.

When a :cpp:class:`elem::Plot` object is declared in a C++ program the instance contains just a pointer to the actual plot object.
The same is also true for :cpp:class:`elem::Window` objects and for all the graphical objects as well.
When a plot or a windows goes out of scope its destructor is invoked but this latter does not immediately delete the actual plot object but it calls instead a special purpose garbage-collector function.
The function, in turn, inspect all the related windows and plots and delete the plot only if no other living object make reference to it.
The same thing is true for windows.

The mechanism just described is nothing less or nothing more than a garbage collector but it is an extremely simple one and is specialized for a very specific use case.
In practice this means that the garbage collector is not normally a concern for the programmer and the :cpp:class:`elem::Plot` and :cpp:class:`elem::Window` objects can be used like an ordinary C++ RAII objects.
