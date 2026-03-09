#include <string>
#include <filesystem>
#include <iostream>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}

#include "engine/lua.hpp"
#include "globals.hpp"

lua_State* L = CE::Scripting::lua; 

int LogInfo(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << "LUA_INFO: " << message << "\n";
    return 0;
}

int LogDebug(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << "LUA_DEBUG: " << message << "\n";
    return 0;
}

int LogWarn(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << "LUA_WARNING: " << message << "\n";
    return 0;
}

int LogError(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << "LUA_ERROR: " << message << "\n";
    return 0;
}



namespace CE::Lua::Functions {

    void ExposeFunctions() {
        if (!L) {
            std::cout << "ERROR: LuaLoader: UNABLE TO EXPOSE FUNCTIONS" "\n";
            return; // safety check
        }
        lua_newtable(L);  // stack: [table]

        lua_pushcfunction(L, LogInfo);
        lua_setfield(L, -2, "Info");

        lua_pushcfunction(L, LogDebug);
        lua_setfield(L, -2, "Debug");

        lua_pushcfunction(L, LogWarn);
        lua_setfield(L, -2, "Warn");

        lua_pushcfunction(L, LogError);
        lua_setfield(L, -2, "Error");

        lua_setglobal(L, "Log"); // pops the table, sets global
        lua_pop(L, 1);
    }

}
