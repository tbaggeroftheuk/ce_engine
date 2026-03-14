#include "engine/lua.hpp"
#include "engine/assets/assets.hpp"
extern "C" {
    #include <raylib.h>
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}

#define LCS luaL_checkstring
#define LCI luaL_checkinteger
#define LCN luaL_checknumber
#define LPB lua_pushboolean
#define LPS lua_pushstring
#define LPN lua_pushnumber

static Color LuaOptColor(lua_State* L, int idx, Color fallback) {
    if (lua_isnoneornil(L, idx)) return fallback;

    if (lua_isuserdata(L, idx)) {
        LuaColor* lc = (LuaColor*)luaL_testudata(L, idx, "ColorMeta");
        if (lc) return lc->c;
        return fallback;
    }

    if (!lua_istable(L, idx)) return fallback;

    Color c = fallback;

    lua_getfield(L, idx, "r");
    c.r = (unsigned char)luaL_optinteger(L, -1, 255);
    lua_pop(L, 1);

    lua_getfield(L, idx, "g");
    c.g = (unsigned char)luaL_optinteger(L, -1, 255);
    lua_pop(L, 1);

    lua_getfield(L, idx, "b");
    c.b = (unsigned char)luaL_optinteger(L, -1, 255);
    lua_pop(L, 1);

    lua_getfield(L, idx, "a");
    c.a = (unsigned char)luaL_optinteger(L, -1, 255);
    lua_pop(L, 1);

    return c;
}

int CE_Load_Font(lua_State* L) {
    const char* Name = LCS(L, 1);
    const char* Path = LCS(L, 2);
    CE::Assets::Fonts::Load(Name, Path);
    return 0;
}

int CE_Load_FontEx(lua_State* L) {
    const char* Name = LCS(L, 1);
    const int Size = LCI(L, 2);
    const char* Path = LCS(L, 3);
    CE::Assets::Fonts::LoadEx(Name, Size, Path);
    return 0;
}

int CE_Unload_Font(lua_State* L) {
    const char* Name = LCS(L, 1);
    CE::Assets::Fonts::Unload(Name);
    return 0;
}

int CE_UnloadAll_Fonts(lua_State* L) {
    CE::Assets::Fonts::UnloadAll();
    return 0;
}

int CE_SetDefault_Font(lua_State* L) {
    const char* Name = LCS(L, 1);
    CE::Assets::Fonts::SetDefaultFont(Name);
    return 0;
}

int CE_Draw_Font(lua_State* L) {
    const char* Text = LCS(L, 1);
    const int Size = LCI(L, 2);
    const int PosX = LCI(L, 3);
    const int PosY = LCI(L, 4);

    Color tint = LuaOptColor(L, 5, BLACK);
    CE::Assets::Fonts::Draw(Text, Size, PosX, PosY, tint);
    return 0;
}

int CE_DrawEx_Font(lua_State* L) {
    const char* Name = LCS(L, 1);
    const char* Text = LCS(L, 2);
    const int Size = LCI(L, 3);
    const int PosX = LCI(L, 4);
    const int PosY = LCI(L, 5);

    Color tint = LuaOptColor(L, 6, BLACK);
    CE::Assets::Fonts::DrawEx(Name, Text, Size, PosX, PosY, tint);
    return 0;
}

namespace CE::Lua::Functions::Assets::Fonts {
    void Register(lua_State* L) {
        lua_newtable(L); // Fonts table

        lua_pushcfunction(L, CE_Load_Font);
        lua_setfield(L, -2, "Load");

        lua_pushcfunction(L, CE_Load_FontEx);
        lua_setfield(L, -2, "LoadEx");

        lua_pushcfunction(L, CE_Unload_Font);
        lua_setfield(L, -2, "Unload");

        lua_pushcfunction(L, CE_UnloadAll_Fonts);
        lua_setfield(L, -2, "UnloadAll");

        lua_pushcfunction(L, CE_SetDefault_Font);
        lua_setfield(L, -2, "SetDefault");

        lua_setglobal(L, "Fonts");

        lua_newtable(L); // Text table

        lua_pushcfunction(L, CE_Draw_Font);
        lua_setfield(L, -2, "Draw");

        lua_pushcfunction(L, CE_DrawEx_Font);
        lua_setfield(L, -2, "DrawEx");

        lua_setglobal(L, "Text");
    }
}
