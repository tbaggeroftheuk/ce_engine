#include "third_party/sol/sol.hpp"
#include <string>
#include <format>
extern "C" {
    #include "raylib.h"
}

#include "engine/lua.hpp"
#include "globals.hpp"

namespace CE::Lua {
    void init() {
        auto& lua = CE::Lua::lua_state;
        lua.open_libraries(
            sol::lib::base,
            sol::lib::math,
            sol::lib::table,
            sol::lib::string,
            sol::lib::utf8
        );
        CE::Lua::Functions::ExposeFunctions();
    }

    void LoadStartup() {
        auto& lua = CE::Lua::lua_state;
        std::string StartupPath = CE::Global.data_path + "startup.lua";
        sol::load_result chunk = lua.load_file(StartupPath);

        if (!chunk.valid()) {
        sol::error err = chunk;
        std::cout << "LUA_VM: Error" << err.what() << "\n";
        return;   
        }
    }
}