#include <sol/sol.hpp>
#include <lualib.h>

#include "elem/elem.h"
#include "elem/elem_lua.h"

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    elem::LuaOpenLibrary(L);
    elem::InitializeFonts();
    sol::state_view lua(L);
    lua.script_file("lua/lua-test-1.lua");
    lua.script_file("lua/lua-test-2.lua");
}
