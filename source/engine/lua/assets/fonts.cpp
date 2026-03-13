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

struct LuaColor {
    Color c;
};

int lua_Color_new(lua_State* L) {
    int r = luaL_optinteger(L, 1, 255);
    int g = luaL_optinteger(L, 2, 255);
    int b = luaL_optinteger(L, 3, 255);
    int a = luaL_optinteger(L, 4, 255);

    LuaColor* lc = (LuaColor*)lua_newuserdata(L, sizeof(LuaColor));
    lc->c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };

    luaL_getmetatable(L, "ColorMeta");
    lua_setmetatable(L, -2);

    return 1; // userdata on stack
}

int lua_Color_tostring(lua_State* L) {
    LuaColor* lc = (LuaColor*)luaL_checkudata(L, 1, "ColorMeta");
    lua_pushfstring(L, "Color(r=%d, g=%d, b=%d, a=%d)", lc->c.r, lc->c.g, lc->c.b, lc->c.a);
    return 1;
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

int CE_Draw_Font(lua_State* L) {
    const char* Text = LCS(L, 1);
    const int Size = LCI(L, 2);
    const int PosX = LCI(L, 3);
    const int PosY = LCI(L, 4);

    Color tint = BLACK;

    if (lua_isuserdata(L, 5)) {
        LuaColor* lc = (LuaColor*)luaL_testudata(L, 4, "ColorMeta");
        if (lc) {
            tint = lc->c;
        }
    }
    CE::Assets::Fonts::Draw(Text, Size, PosX, PosY, tint);
    return 0;
}

int CE_DrawEx_Font(lua_State* L) {
    const char* Name = LCS(L, 1);
    const char* Text = LCS(L, 2);
    const int Size = LCI(L, 3);
    const int PosX = LCI(L, 3);
    const int PosY = LCI(L, 4);

    Color tint = BLACK;

    if (lua_isuserdata(L, 6)) {
        LuaColor* lc = (LuaColor*)luaL_testudata(L, 4, "ColorMeta");
        if (lc) {
            tint = lc->c;
        }
    }
    CE::Assets::Fonts::Draw(Text, Size, PosX, PosY, tint);
    return 0;
}

namespace CE::Lua::Functions::Assets::Fonts {
    void Register(lua_State* L) {
        lua_newtable(L); // Fonts table

        lua_pushcfunction(L, CE_Load_Font);
        lua_setfield(L, -2, "Load");

        lua_pushcfunction(L, CE_Load_FontEx);
        lua_setfield(L, -2, "LoadEx");

        lua_setglobal(L, "Fonts");

        lua_newtable(L); // Text table

        lua_pushcfunction(L, CE_Draw_Font);
        lua_setfield(L, -2, "Draw");

        lua_pushcfunction(L, CE_DrawEx_Font);
        lua_setfield(L, -2, "DrawEx");

        lua_setglobal(L, "Text");
    }
}