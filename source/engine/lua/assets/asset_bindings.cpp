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

void RegisterColor(lua_State* L) {
    luaL_newmetatable(L, "ColorMeta");

    lua_pushcfunction(L, lua_Color_tostring);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1); // pop metatable

    lua_pushcfunction(L, lua_Color_new);
    lua_setglobal(L, "Color"); // now Lua can do color(r,g,b,a)
}

static int lua_Vec2(lua_State* L) {
    const double x = luaL_optnumber(L, 1, 0.0);
    const double y = luaL_optnumber(L, 2, 0.0);

    lua_newtable(L);
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    return 1;
}

static int lua_Rect(lua_State* L) {
    const double x = luaL_optnumber(L, 1, 0.0);
    const double y = luaL_optnumber(L, 2, 0.0);
    const double w = luaL_optnumber(L, 3, 0.0);
    const double h = luaL_optnumber(L, 4, 0.0);

    lua_newtable(L);
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, w);
    lua_setfield(L, -2, "w");
    lua_pushnumber(L, h);
    lua_setfield(L, -2, "h");
    return 1;
}

static int lua_Draw_Clear(lua_State* L) {
    const Color color = LuaOptColor(L, 1, BLACK);
    ClearBackground(color);
    return 0;
}

static int lua_Draw_Rect(lua_State* L) {
    const int x = LCI(L, 1);
    const int y = LCI(L, 2);
    const int w = LCI(L, 3);
    const int h = LCI(L, 4);
    const Color color = LuaOptColor(L, 5, WHITE);
    DrawRectangle(x, y, w, h, color);
    return 0;
}

static int lua_Draw_Circle(lua_State* L) {
    const int x = LCI(L, 1);
    const int y = LCI(L, 2);
    const float r = (float)LCN(L, 3);
    const Color color = LuaOptColor(L, 4, WHITE);
    DrawCircle(x, y, r, color);
    return 0;
}

static int lua_Draw_Line(lua_State* L) {
    const int x1 = LCI(L, 1);
    const int y1 = LCI(L, 2);
    const int x2 = LCI(L, 3);
    const int y2 = LCI(L, 4);
    const Color color = LuaOptColor(L, 5, WHITE);
    DrawLine(x1, y1, x2, y2, color);
    return 0;
}

namespace CE::Lua::Functions::Assets {
        void Register(lua_State* L) {
            // --- Color ---
            RegisterColor(L);

            // --- Primitive helpers ---
            lua_pushcfunction(L, lua_Vec2);
            lua_setglobal(L, "Vec2");

            lua_pushcfunction(L, lua_Rect);
            lua_setglobal(L, "Rect");

            // --- Draw primitives ---
            lua_newtable(L); // Draw table

            lua_pushcfunction(L, lua_Draw_Clear);
            lua_setfield(L, -2, "Clear");

            lua_pushcfunction(L, lua_Draw_Rect);
            lua_setfield(L, -2, "Rect");

            lua_pushcfunction(L, lua_Draw_Circle);
            lua_setfield(L, -2, "Circle");

            lua_pushcfunction(L, lua_Draw_Line);
            lua_setfield(L, -2, "Line");

            lua_setglobal(L, "Draw");
        }
}
