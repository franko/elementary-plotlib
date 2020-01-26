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
        .beginClass<Path>("Path")
            .addConstructor<void (*)(void)>()
            .addFunction("MoveTo", &Path::MoveTo)
            .addFunction("LineTo", &Path::LineTo)
            .addFunction("ClosePolygon", &Path::ClosePolygon)
        .endClass()
    .endNamespace();

    luaL_dofile(L, "lua/bridge-test.lua");
    return 0;
}
