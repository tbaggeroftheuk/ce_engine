#include <string>
#include <filesystem>
#include <iostream>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}

#include "engine/lua.hpp"
#include "engine/assets/assets.hpp"
#include "globals.hpp"

#define LCS luaL_checkstring
#define LCI luaL_checkinteger
#define LCN luaL_checknumber
#define LPB lua_pushboolean
#define LPS lua_pushstring
#define LPN lua_pushnumber

#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_CYAN    "\033[36m"

#define LNTBLE lua_newtable

lua_State* L = CE::Scripting::lua; 

int LogInfo(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << ANSI_BLUE << "LUA_INFO: " << message << ANSI_RESET << "\n";
    return 0;
}

int LogDebug(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << ANSI_CYAN << "LUA_DEBUG: " << message << ANSI_RESET << "\n";
    return 0;
}

int LogWarn(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << ANSI_YELLOW << "LUA_WARNING: " << message << ANSI_RESET << "\n";
    return 0;
}

int LogError(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    std::cout << ANSI_RED << "LUA_ERROR: " << message << ANSI_RESET << "\n";
    return 0;
}

namespace CE::Lua::Functions {

    void ExposeFunctions() {
        if (!L) {
            std::cout << "ERROR: LuaLoader: UNABLE TO EXPOSE FUNCTIONS" "\n";
            return; // safety check
        }

        lua_newtable(L); // stack: [LogTable]

        lua_pushcfunction(L, LogInfo);
        lua_setfield(L, -2, "Info");

        lua_pushcfunction(L, LogDebug);
        lua_setfield(L, -2, "Debug");

        lua_pushcfunction(L, LogWarn);
        lua_setfield(L, -2, "Warn");

        lua_pushcfunction(L, LogError);
        lua_setfield(L, -2, "Error");

        lua_setglobal(L, "Log"); // pops the table
    }

}
