#include "libelplot_lua_c.h"
#include "libelplot_lua_cpp.h"

void elp_lua_open_library(lua_State *L) {
    elp::LuaOpenLibrary(L);
}
