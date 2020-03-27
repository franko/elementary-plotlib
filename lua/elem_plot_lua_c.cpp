#include "elem/elem_lua_c.h"
#include "elem/elem_lua_cpp.h"

void elem_lua_open_library(lua_State *L) {
    elem::LuaOpenLibrary(L);
}
