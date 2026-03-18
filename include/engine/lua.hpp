#pragma once

extern "C" {
    #include <raylib.h>
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}
int lua_Color_new(lua_State* L);
int lua_Color_tostring(lua_State* L);

struct LuaColor {
    Color c;
};

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

namespace CE::Lua::Functions::Primitives {
    void Register(lua_State* L);
}

namespace CE::Lua::Functions::Assets::Textures {
    void Register(lua_State* L);
}

namespace CE::Lua::Functions::Assets::Audio {
    void Register(lua_State* L);
}


namespace CE::Lua::Functions::Assets::Fonts {
    void Register(lua_State* L);
}

namespace CE::Lua::Functions::Time {
    void Register(lua_State* L);
}

namespace CE::Lua::Functions::Misc {
    void Register(lua_State* L);
}

namespace CE::Lua::Functions::Data {
    void Register(lua_State* L);
}

namespace CE::Lua::Functions::Callbacks {
    void Register(lua_State* L);
}