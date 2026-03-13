#include "engine/lua.hpp"
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

void RegisterColor(lua_State* L) {
    luaL_newmetatable(L, "ColorMeta");

    lua_pushcfunction(L, lua_Color_tostring);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1); // pop metatable

    lua_pushcfunction(L, lua_Color_new);
    lua_setglobal(L, "Color"); // now Lua can do color(r,g,b,a)
}

int TextureLoad(lua_State* L) {
    const char* Name = LCS(L, 1);
    const char* Path = LCS(L, 2);

    CE::Assets::Textures::Load(Name, Path);
    return 0;
}

int TextureLoadFolder(lua_State* L) {
    const char* FolderPath = LCS(L, 1);
    CE::Assets::Textures::LoadFolder(FolderPath);
    return 0;
}

int TexturesDraw(lua_State* L) {
    const char* name = LCS(L, 1);
    const int posX = LCI(L, 2);
    const int posY = LCI(L, 3);

    Color tint = WHITE;

    if (lua_isuserdata(L, 4)) {
        LuaColor* lc = (LuaColor*)luaL_testudata(L, 4, "ColorMeta");
        if (lc) {
            tint = lc->c;
        }
    } 
    else if (lua_istable(L, 4)) {
        lua_getfield(L, 4, "r");
        tint.r = luaL_optinteger(L, -1, 255);
        lua_pop(L, 1);

        lua_getfield(L, 4, "g");
        tint.g = luaL_optinteger(L, -1, 255);
        lua_pop(L, 1);

        lua_getfield(L, 4, "b");
        tint.b = luaL_optinteger(L, -1, 255);
        lua_pop(L, 1);

        lua_getfield(L, 4, "a");
        tint.a = luaL_optinteger(L, -1, 255);
        lua_pop(L, 1);
    }

    CE::Assets::Textures::Draw(name, posX, posY, tint);
    return 0;
}

int AudioMusicLoad(lua_State* L) {
    const char* Path = LCS(L, 1);
    const char* Name = LCS(L, 2);

    CE::Assets::Audio::LoadMusic(Name, Path);
    return 0;
}

int AudioMusicPlay(lua_State* L) {
    const char* Name = LCS(L, 1);
    CE::Assets::Audio::PlayMusic(Name);
    return 0;
}

int AudioMusicPause(lua_State* L) {
    const char* Name = LCS(L, 1);
    CE::Assets::Audio::PauseMusic(Name);
    return 0;
}

int AudioMusicResume(lua_State* L) {
    const char* Name = LCS(L, 1);
    CE::Assets::Audio::ResumeMusic(Name);
    return 0;
}

int AudioMusicUnload(lua_State* L) {
    const char* Name = LCS(L, 1);
    CE::Assets::Audio::UnloadMusic(Name);
    return 0;
}

int AudioMusicUnloadAll(lua_State* L) {
    CE::Assets::Audio::UnloadAllMusic();
    return 0;
}

namespace CE::Lua::Functions::Assets {
        void Register(lua_State* L) {
            // --- Color ---
            RegisterColor(L);

            // --- Textures Table ---
            lua_newtable(L); // stack: [TexturesTable]

            lua_pushcfunction(L, TexturesDraw);
            lua_setfield(L, -2, "Draw");

            lua_pushcfunction(L, TextureLoad);
            lua_setfield(L, -2, "Load");

            lua_pushcfunction(L, TextureLoadFolder);
            lua_setfield(L, -2, "LoadFolder");

            lua_setglobal(L, "Textures"); // pops the table

            lua_newtable(L); // Audio table

            lua_newtable(L); // Music table

            lua_pushcfunction(L, AudioMusicLoad);
            lua_setfield(L, -2, "Load");

            lua_pushcfunction(L, AudioMusicUnload);
            lua_setfield(L, -2, "Unload");

            lua_pushcfunction(L, AudioMusicPlay);
            lua_setfield(L, -2, "Play");

            lua_pushcfunction(L, AudioMusicPause);
            lua_setfield(L, -2, "Pause");

            lua_pushcfunction(L, AudioMusicResume);
            lua_setfield(L, -2, "Resume");

            lua_pushcfunction(L, AudioMusicUnloadAll);
            lua_setfield(L, -2, "UnloadAll");

            lua_setfield(L, -2, "Music"); // ← FIX HERE

            lua_setglobal(L, "Audio");

        }
}