Lua Bindings
============

C++ Loader Function
-------------------

The Elementary Plot library can be used from C or C++ applications using the Lua programming language.
The functions described below are C/C++ functions that loads the Elementary Plot library's functions into a ``lua_State`` pointer.

.. cpp:namespace:: elp

.. cpp:function:: void LuaOpenLibrary(lua_State *L)

  For the given Lua state, load the library's functions and classes into a global Lua table named ``elp``.

  .. highlight:: lua

  Here a Lua example that illustrate how to use the library from Lua::

    local plot = elp.Plot.new(elp.plot.ShowUnits)
    plot:SetLimits(-1.0, 0.0, 1.0, 10.0)
    plot:SetAxisLabelsAngle(elp.xAxis, 3.141592 / 4)
    plot:EnableLabelFormat(elp.xAxis, "%.6f")

    local line = elp.Path.new()
    line:MoveTo(-0.5, 0)
    line:LineTo(-0.5, 8.0)
    line:LineTo(0.5, 4.0)
    line:ClosePolygon()
    plot:Add(line, 0xB40000FF, 2.5, 0xF5FE00FF, elp.flags(elp.property.Stroke, elp.property.Fill))

    local window = elp.Window.new()
    window:Attach(plot, "")

    window:Start(640, 480, elp.WindowResize)
    elp.utils.Sleep(4)

    local line2 = elp.Path.new()
    line2:MoveTo(0.8, 1.0)
    line2:LineTo(0.8, 7.0)
    line2:LineTo(0.3, 4.0)
    line2:ClosePolygon()
    plot:Add(line2, 0x0000B4FF, 2.5, 0x00000000, elp.property.Stroke)

    window:Wait()

C Loader Function
-----------------

.. c:function:: void elp_lua_open_library(lua_State *L)

  The same of the :cpp:func:`LuaOpenLibrary` but as a C function.
