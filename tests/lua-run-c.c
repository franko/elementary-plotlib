#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
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
    elem_lua_open_library(L);
    elem_initialize_fonts();
    int status;
    status = luaL_loadfile(L, argv[1]);
    if (status == 0) {
        lua_pcall(L, 0, 0, 0);
    }
    lua_close(L);
    return 0;
}
