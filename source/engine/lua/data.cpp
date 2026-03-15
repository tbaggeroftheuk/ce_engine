#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h" 
    #include "lualib.h" 
}

#include "engine/lua.hpp"
#include "globals.hpp"

int CE_Open_File(lua_State* L) {
    const char* filePath = luaL_checkstring(L, 1);
    std::string fileName = CE::Global.data_path + filePath;
    
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "LUA_ERROR: Failed to open file: " << fileName << "\n";
        return 0;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContents = buffer.str();

    lua_pushstring(L, fileContents.c_str());
    return 1;
}

namespace CE::Lua::Functions::Data {
    void Register(lua_State* L) {
        lua_newtable(L); // Table "Data"
        
        lua_pushcfunction(L, CE_Open_File);
        lua_setfield(L, -2, "GetFile");

        lua_setglobal(L, "Data");
    }
}