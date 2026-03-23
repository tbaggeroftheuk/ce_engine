extern "C" {
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
    #include "raylib.h"
}

#include "globals.hpp"
#include "engine/lua.hpp"

int CE_Window_GetSize(lua_State* L) {
    lua_pushinteger(L, CE::Global.window_width);
    lua_pushinteger(L, CE::Global.window_height);
    return 2;
}

int CE_Window_SetSize(lua_State* L) {
    const int Width = luaL_checkinteger(L, 1);
    const int Height = luaL_checkinteger(L, 2);
    SetWindowSize(Width, Height);
    return 0;
}

int CE_Window_SetTitle(lua_State* L) {
    const char* Title = luaL_checkstring(L, 1);
    SetWindowTitle(Title);
    return 0;
}

namespace CE::Lua::Functions::Window {
    void Register(lua_State* L) {

    }
}