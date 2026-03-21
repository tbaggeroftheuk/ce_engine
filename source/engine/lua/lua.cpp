#include <string>
#include <format>
#include <iostream>

extern "C" {
    #include <raylib.h>
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}
#include "engine/common/error_box.hpp"
#include "engine/lua.hpp"
#include "globals.hpp"
#include "engine/common.hpp"

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

        // Make it so you can only require from the CE::Global.data_path
        lua_getglobal(L, "package");   
        std::string luaPath = CE::Global.data_path + "/?.lua;" + CE::Global.data_path + "?/init.lua";
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

        lua_pushnil(L);
        lua_setglobal(L, "dofile");

        lua_pushnil(L);
        lua_setglobal(L, "loadfile");

        lua_pushcfunction(L, [](lua_State* L) -> int { // This mf is to sandbox dofile
            const char* filename = luaL_checkstring(L, 1);
            std::string fullPath = CE::Global.data_path + filename;

            if (fullPath.find(CE::Global.data_path) != 0) {
                return luaL_error(L, "Access denied");
            }

            if (luaL_loadfile(L, fullPath.c_str()) != LUA_OK) {
                return lua_error(L);
            }

            return lua_pcall(L, 0, LUA_MULTRET, 0);
        });
        lua_setglobal(L, "dofile");

        lua_pushcfunction(L, [](lua_State* L) -> int { // Also do sandbox loadfile
            const char* filename = luaL_checkstring(L, 1);

            std::string fullPath = CE::Global.data_path + filename;
            if (fullPath.find(CE::Global.data_path) != 0) {
                return luaL_error(L, "Access denied outside sandbox");
            }
            if (luaL_loadfile(L, fullPath.c_str()) != LUA_OK) {
                return lua_error(L);
            }

            lua_newtable(L);            
            lua_setupvalue(L, -2, 1);  
            return lua_pcall(L, 0, LUA_MULTRET, 0);
        });
        lua_setglobal(L, "loadfile");

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
            const char* lua_msg = lua_tostring(L, -1);
            std::string lua_error = "LUA_ERROR: Runtime error: ";
            if (lua_msg) {
                lua_error += lua_msg;
            } else {
                lua_error += "(no message)";
            }

            for (char& c : lua_error) {
                if (c == '"') c = '\'';
                else if (c == '\n' || c == '\r') c = ' '; 
                else if (c < 32 || c > 126) c = '?';
            }
            std::cerr << lua_error <<"\n";
            CloseWindow();
            ShowError("Scripting error, check logs!");  
            lua_pop(L, 1); 
            CE::Shutdown(2);
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
