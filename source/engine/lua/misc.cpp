extern "C" {
    #include <raylib.h>
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}

#include "engine/lua.hpp"

#define LCS luaL_checkstring
#define LCI luaL_checkinteger
#define LCN luaL_checknumber
#define LPB lua_pushboolean
#define LPS lua_pushstring
#define LPN lua_pushnumber

int CE_OpenURL(lua_State* L) {
    const char* url = LCS(L, 1);
    OpenURL(url);
    return 0;
}

namespace CE::Lua::Functions::Misc {
    void Register(lua_State* L) {
        lua_newtable(L);

        lua_pushcfunction(L, CE_OpenURL);
        lua_setfield(L, -2, "OpenURL");

        lua_setglobal(L, "Misc");
    }
}