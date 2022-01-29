#include "l_cafe.h"

lua_State *L;

int luaopen_cafe(lua_State *L) {
    L = luaL_newstate();
    return 1;
}