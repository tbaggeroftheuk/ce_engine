#include "engine/lua.hpp"
#include <chrono>

auto& lua = CE::Lua::lua_state;

void LuaTimeOutHook(lua_State* L, lua_Debug*) {

    auto* start =
        (std::chrono::steady_clock::time_point*)lua_getextraspace(L);

    auto now = std::chrono::steady_clock::now();

    if (now - *start > std::chrono::milliseconds(5))
        luaL_error(L, "Lua: too long without yielding");
}

namespace CE::Lua::Functions {

void ExposeFunctions() {

    lua_sethook(lua.lua_state(), LuaTimeOutHook, LUA_MASKCOUNT, 10000);

    lua.set_function("wait", [](float seconds) {
        return sol::yielding(seconds);
    });

}

}