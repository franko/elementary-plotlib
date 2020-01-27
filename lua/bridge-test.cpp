/* Using resources from:

  https://github.com/vinniefalco/LuaBridge
  http://vinniefalco.github.io/LuaBridge/Manual.html
  https://eliasdaler.wordpress.com/2014/07/18/using-lua-with-cpp-luabridge/
  https://www.lua.org/manual/5.1/manual.html#luaL_newstate
*/

#include "libelplot.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

using namespace elp;
using namespace luabridge;

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    getGlobalNamespace(L).beginNamespace("elem")
        .beginClass<Object>("Object")
        .endClass()
        .deriveClass<Path, Object>("Path")
            .addConstructor<void (*)(void)>()
            .addFunction("MoveTo", &Path::MoveTo)
            .addFunction("LineTo", &Path::LineTo)
            .addFunction("ClosePolygon", &Path::ClosePolygon)
        .endClass()
        .beginClass<Plot>("Plot")
            .addConstructor<void (*)(unsigned)>()
            .addFunction("SetTitle", &Plot::SetTitle)
            .addFunction("SetXAxisTitle", &Plot::SetXAxisTitle)
            .addFunction("SetYAxisTitle", &Plot::SetYAxisTitle)
            .addFunction("SetClipMode", &Plot::SetClipMode)
            .addFunction("SetLimits", &Plot::SetLimits)
            .addFunction("SetAxisLabelsAngle", &Plot::SetAxisLabelsAngle)
            .addFunction("EnableLabelFormat", &Plot::EnableLabelFormat)
            .addFunction("Add", &Plot::Add)
            .addFunction("AddStroke", &Plot::AddStroke)
            .addFunction("PushLayer", &Plot::PushLayer)
            .addFunction("PopLayer", &Plot::PopLayer)
            .addFunction("AddLegend", &Plot::AddLegend)
            .addFunction("WriteSvg", &Plot::WriteSvg)
        .endClass()
        .beginClass<Window>("Window")
            .addConstructor<void (*)(const char *)>()
            .addFunction("Attach", &Window::Attach)
            .addFunction("SetLayout", &Window::SetLayout)
            .addFunction("SlotRefresh", &Window::SlotRefresh)
            .addFunction("Start", &Window::Start)
            .addFunction("Wait", &Window::Wait)
        .endClass()
    .endNamespace();

    luaL_dofile(L, "lua/bridge-test.lua");
    return 0;
}
