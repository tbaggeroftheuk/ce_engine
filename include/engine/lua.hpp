#pragma once

extern "C" {
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}
namespace CE::Scripting {
    inline lua_State* lua = luaL_newstate();
}

namespace CE::Lua::Functions {
    void ExposeFunctions();
}

namespace CE::Lua {
    void init();
    void LoadStartup();
    void LuaUpdate(); 
}