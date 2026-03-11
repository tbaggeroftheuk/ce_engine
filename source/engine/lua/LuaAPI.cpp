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

        lua_newtable(L); // stack: [AudioTable]

        lua_pushcfunction(L, AudioMusicLoad);
        lua_setfield(L, -2, "MusicLoad");

        lua_pushcfunction(L, AudioMusicPlay);
        lua_setfield(L, -2, "MusicPlay");

        lua_setglobal(L, "Audio"); // pops the table

    }

}
