#include <string>
#include <format>
#include <iostream>

extern "C" {
    #include <raylib.h>
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}

#include "engine/lua.hpp"
#include "globals.hpp"

int g_luaUpdateRef = LUA_NOREF; // global reference to Update()

// Helper function to remove stuff from a table
void LuaMakeNil(lua_State* L, const char* tableName, const char* key) {
    lua_getglobal(L, tableName); // push table
    if (lua_istable(L, -1)) {
        lua_pushstring(L, key);   // push key
        lua_pushnil(L);           // push nil
        lua_settable(L, -3);      // table[key] = nil
    }
    lua_pop(L, 1);               // pop the table
}

// Helper function to remove an entire table
void LuaRemoveTable(lua_State* L, const char* tableName) {
    lua_pushnil(L); // nil to the stack
    lua_setglobal(L, tableName); // pop the table
}

namespace CE::Lua {
    void init() {
        lua_State* L = CE::Scripting::lua; // alias....
        luaL_openlibs(CE::Scripting::lua);

        LuaRemoveTable(L, "debug");
        LuaRemoveTable(L, "io");
        LuaRemoveTable(L, "os");
        LuaRemoveTable(L, "ffi");

        lua_getglobal(L, "package");

        // Make it so you can only require from the CE::Global.data_path
        std::string luaPath = CE::Global.data_path + "?.lua;" + CE::Global.data_path + "?/init.lua";
        lua_pushstring(L, "path");        
        lua_pushstring(L, luaPath.c_str()); 
        lua_settable(L, -3);           
        
        // Block native modules aka so/dll
        lua_getglobal(L, "package");
        lua_pushstring(L, "cpath");
        lua_pushstring(L, "");
        lua_settable(L, -3);
        lua_pop(L, 1);

        lua_newtable(L);

        CE::Lua::Functions::ExposeFunctions();
        CE::Lua::Functions::ce_functions::Register(L);
        CE::Lua::Functions::RaylibBindings::Register(L);
        CE::Lua::Functions::Primitives::Register(L);
        CE::Lua::Functions::Assets::Textures::Register(L);
        CE::Lua::Functions::Assets::Audio::Register(L);
        CE::Lua::Functions::Assets::Fonts::Register(L);
        CE::Lua::Functions::Time::Register(L);
        CE::Lua::Functions::Misc::Register(L);
        CE::Lua::Functions::Data::Register(L);
        CE::Lua::Functions::Callbacks::Register(L);
        
    }

    void LoadStartup() {
        lua_State* L = CE::Scripting::lua;
        std::string startupPath = CE::Global.data_path + "/startup.lua";

        int status = luaL_loadfile(L, startupPath.c_str());

        if (status != LUA_OK) {
            std::cerr << "Lua load error: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1);
            return;
        }

        status = lua_pcall(L, 0, LUA_MULTRET, 0);

        // Try to get global Update function
        lua_getglobal(L, "Update"); // pushes Update onto stack
        if (lua_isfunction(L, -1)) {
            g_luaUpdateRef = luaL_ref(L, LUA_REGISTRYINDEX); // stores ref, pops from stack
        } else {
            
            lua_pop(L, 1); // pop nil
        }

        if (status != LUA_OK) {
            std::cerr << "LUA_ERROR: Runtime error: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1);
        }
    }

    void LuaUpdate() {
        lua_State* L = CE::Scripting::lua;

        if (g_luaUpdateRef == LUA_NOREF) return; // nothing to call

        lua_rawgeti(L, LUA_REGISTRYINDEX, g_luaUpdateRef); // push the function

        // call it with 0 args, 0 return values
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            std::cerr << "Lua Update error: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1);
        }
    }

    void Shutdown() {
        lua_close(CE::Scripting::lua);
    }
}
