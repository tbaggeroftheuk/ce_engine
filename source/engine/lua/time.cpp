#include "engine/lua.hpp"
#include <chrono>

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

int CE_GetOSTime(lua_State* L) {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    lua_pushnumber(L, static_cast<lua_Number>(currentTime));
    return 1;
}

namespace CE::Lua::Functions::Time {
    void Register(lua_State* L) {
        lua_newtable(L); // Table "Time"
        
        lua_pushcfunction(L, CE_GetDeltaTime);
        lua_setfield(L, -2, "Delta");

        lua_pushcfunction(L, CE_GetTime);
        lua_setfield(L, -2, "Time");

        lua_pushcfunction(L, CE_GetOSTime);
        lua_setfield(L, -2, "OS");

        lua_setglobal(L, "Time");
    }
}
