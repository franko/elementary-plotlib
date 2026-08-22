#include <stdio.h>

#include <sol/sol.hpp>
#include <lualib.h>

#include "elem/elem.h"
#include "elem/elem_lua.h"

// The ELEM_USER_MAIN macro declares a function taking no arguments so we
// cannot use it here. We call InitializeAndRun directly instead and pass the
// script's filename using the variable below.
static const char *g_script_file;

static int RunScript() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    elem::LuaOpenLibrary(L);
    elem::InitializeFonts();
    sol::state_view lua(L);
    lua.script_file(g_script_file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <lua-file>\n", argv[0]);
        return 1;
    }
    g_script_file = argv[1];
    return elem::InitializeAndRun(RunScript);
}
