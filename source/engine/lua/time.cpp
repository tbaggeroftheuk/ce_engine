#include "engine/lua.hpp"

extern "C" {
    #include "raylib.h"
}

int CE_GetTime(lua_State* L) {
    lua_pushnumber(L, GetTime());
    return 1;
}

int CE_GetDeltaTime(lua_State* L) {
    lua_pushnumber(L, GetFrameTime());
    return 1;
}

namespace CE::Lua::Functions::Time {
    void Register(lua_State* L) {
        lua_newtable(L); // Table "Time"
        
        lua_pushcfunction(L, CE_GetDeltaTime);
        lua_setfield(L, -2, "Delta");

        lua_pushcfunction(L, CE_GetTime);
        lua_setfield(L, -2, "Time");

        lua_setglobal(L, "Time");
    }
}
