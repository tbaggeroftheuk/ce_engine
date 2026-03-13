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

namespace CE::Lua::Functions::RaylibBindings {
    void Register(lua_State* L);
}

namespace CE::Lua {
    void init();
    void LoadStartup();
    void LuaUpdate(); 
    void Shutdown();
}

namespace CE::Lua::Functions::ce_functions {
    void Register(lua_State* L);
}

namespace CE::Lua::Functions::Assets {
    void Register(lua_State* L);
}


namespace CE::Lua::Functions::Assets::Fonts {
    void Register(lua_State* L);
}