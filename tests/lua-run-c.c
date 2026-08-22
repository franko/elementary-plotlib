#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "elem/elem.h"
#include "elem/elem_lua.h"

/* The ELEM_USER_MAIN macro declares a function taking no arguments so we
   cannot use it here. We call elem_initialize_and_run directly instead and
   pass the script's filename using the variable below. */
static const char *g_script_file;

static int run_script(void) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    elem_lua_open_library(L);
    elem_initialize_fonts();
    int status;
    status = luaL_loadfile(L, g_script_file);
    if (status == 0) {
        lua_pcall(L, 0, 0, 0);
    }
    lua_close(L);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <lua-file>\n", argv[0]);
        return 1;
    }
    g_script_file = argv[1];
    return elem_initialize_and_run(run_script);
}
