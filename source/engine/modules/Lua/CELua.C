#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <globals.h>

void CELua_Initialize(void) {
    ce_globals.Lua = luaL_newstate();
    if (!ce_globals.Lua) {
        TraceLog(LOG_ERROR, "Failed to create Lua state");
        exit(1);
    }

    luaL_requiref(ce_globals.Lua, "_G", luaopen_base, 1); // Because I'm not stupid I'm trying to sandbox lua 
    lua_pop(ce_globals.Lua, 1);

    luaL_requiref(ce_globals.Lua, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(ce_globals.Lua, 1);

    luaL_requiref(ce_globals.Lua, LUA_STRLIBNAME, luaopen_string, 1);
    lua_pop(ce_globals.Lua, 1);

    luaL_requiref(ce_globals.Lua, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(ce_globals.Lua, 1);

    luaL_requiref(ce_globals.Lua, LUA_UTF8LIBNAME, luaopen_utf8, 1);
    lua_pop(ce_globals.Lua, 1);
}

bool CELua_RunFile(const char *file_path) {
    char path_sandboxed[PATH_MAX_LEN];
    snprintf(path_sandboxed, sizeof(path_sandboxed), "%s", file_path);

    if (luaL_dofile(ce_globals.Lua, path_sandboxed) != LUA_OK)  {
        TraceLog(LOG_INFO, "CELua  ERROR: %s", lua_tostring(ce_globals.Lua, -1));
        lua_pop(ce_globals.Lua, 1);
        return false;
    }
    return true;
}
