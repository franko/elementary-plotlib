#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "libelplot.h"
#include "libelplot_lua.h"

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    elp_lua_open_library(L);
    elp_initialize_fonts();
    int status;
    status = luaL_loadfile(L, "lua/lua-test-1.lua");
    if (status == 0) {
        lua_pcall(L, 0, 0, 0);
    }
    status = luaL_loadfile(L, "lua/lua-test-2.lua");
    if (status == 0) {
        lua_pcall(L, 0, 0, 0);
    }
    lua_close(L);
    return 0;
}
