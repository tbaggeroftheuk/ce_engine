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

static int CE_Texture_Load(lua_State* L) {
    const char* name = LCS(L, 1);
    const char* path = LCS(L, 2);
    CE::Assets::Textures::Load(name, path);
    return 0;
}

static int CE_Texture_LoadFolder(lua_State* L) {
    const char* folderPath = LCS(L, 1);
    CE::Assets::Textures::LoadFolder(folderPath);
    return 0;
}

static int CE_Texture_Draw(lua_State* L) {
    const char* name = LCS(L, 1);
    const int posX = LCI(L, 2);
    const int posY = LCI(L, 3);
    const Color tint = LuaOptColor(L, 4, WHITE);
    CE::Assets::Textures::Draw(name, posX, posY, tint);
    return 0;
}

static int CE_Texture_Exists(lua_State* L) {
    const char* name = LCS(L, 1);
    lua_pushboolean(L, CE::Assets::Textures::Exists(name));
    return 1;
}

static int CE_Texture_Loaded(lua_State* L) {
    lua_pushinteger(L, CE::Assets::Textures::LoadedTextures());
    return 1;
}

static int CE_Texture_LoadedNoError(lua_State* L) {
    lua_pushinteger(L, CE::Assets::Textures::LoadedTexturesNoError());
    return 1;
}

static int CE_Texture_LoadedError(lua_State* L) {
    lua_pushinteger(L, CE::Assets::Textures::LoadedTexturesError());
    return 1;
}

static int CE_Texture_Unload(lua_State* L) {
    const char* name = LCS(L, 1);
    CE::Assets::Textures::Unload(name);
    return 0;
}

static int CE_Texture_UnloadAll(lua_State* L) {
    (void)L;
    CE::Assets::Textures::UnloadAll();
    return 0;
}

namespace CE::Lua::Functions::Assets::Textures {
    void Register(lua_State* L) {
        lua_newtable(L); // Textures table

        lua_pushcfunction(L, CE_Texture_Load);
        lua_setfield(L, -2, "Load");

        lua_pushcfunction(L, CE_Texture_LoadFolder);
        lua_setfield(L, -2, "LoadFolder");

        lua_pushcfunction(L, CE_Texture_Draw);
        lua_setfield(L, -2, "Draw");

        lua_pushcfunction(L, CE_Texture_Unload);
        lua_setfield(L, -2, "Unload");

        lua_pushcfunction(L, CE_Texture_UnloadAll);
        lua_setfield(L, -2, "UnloadAll");

        lua_pushcfunction(L, CE_Texture_Exists);
        lua_setfield(L, -2, "Exists");

        lua_pushcfunction(L, CE_Texture_Loaded);
        lua_setfield(L, -2, "Loaded");

        lua_pushcfunction(L, CE_Texture_LoadedNoError);
        lua_setfield(L, -2, "LoadedNoError");

        lua_pushcfunction(L, CE_Texture_LoadedError);
        lua_setfield(L, -2, "LoadedError");

        lua_setglobal(L, "Textures");
    }
}
