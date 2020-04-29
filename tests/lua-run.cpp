#include <stdio.h>

#include <sol/sol.hpp>
#include <lualib.h>

#include "elem/elem.h"
#include "elem/elem_lua.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <lua-file>\n", argv[0]);
        return 1;
    }
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    elem::LuaOpenLibrary(L);
    elem::InitializeFonts();
    sol::state_view lua(L);
    lua.script_file(argv[1]);
    return 0;
}
