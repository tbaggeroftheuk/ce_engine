extern "C" {
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}

#include "engine/lua.hpp"
#include "engine/callbacks.hpp"
#include "globals.hpp"

#define LCS luaL_checkstring
#define LCI luaL_checkinteger
#define LCN luaL_checknumber
#define LPB lua_pushboolean
#define LPS lua_pushstring
#define LPN lua_pushnumber

int GetGameName(lua_State* L) {
    lua_pushstring(L, CE::game_name.c_str());
    return 1;
}

int GetGameVersion(lua_State* L) {
    lua_pushstring(L, CE::game_ver.c_str());
    return 1;
}

int IsDebugOn(lua_State* L) {
    lua_pushboolean(L, CE::Debug);
    return 1;
}

int Shutdown_CE(lua_State* L) {
    CE::should_exit = true;
    return 0;
}

int SetGameState_CE(lua_State* L) {
    const char* name = LCS(L, 1);
    if (!name || !*name) {
        CE::currentGameStateName = "None";
        CE::Callbacks::SetState(CE::currentGameStateName);
        return 0;
    }
    CE::currentGameStateName = name;
    CE::Callbacks::SetState(CE::currentGameStateName);
    return 0;
}

int GetGameState_CE(lua_State* L) {
    lua_pushstring(L, CE::currentGameStateName.c_str());
    return 1;
}

namespace CE::Lua::Functions::ce_functions {
    void Register(lua_State* L) {
        lua_newtable(L);

        lua_pushcfunction(L, GetGameName);
        lua_setfield(L, -2, "GetGameName");

        lua_pushcfunction(L, GetGameVersion);
        lua_setfield(L, -2, "GetGameVersion");

        lua_pushcfunction(L, IsDebugOn);
        lua_setfield(L, -2, "IsDebugOn");

        lua_pushcfunction(L, Shutdown_CE);
        lua_setfield(L, -2, "Shutdown");

        lua_pushcfunction(L, SetGameState_CE);
        lua_setfield(L, -2, "SetGameState");

        lua_pushcfunction(L, GetGameState_CE);
        lua_setfield(L, -2, "GetGameState");

        lua_setglobal(L, "CE");
    }
}
