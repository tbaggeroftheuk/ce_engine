#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>

#include "globals.hpp"

namespace CE::Callbacks {
    using LuaDispatcher = std::function<void(const std::string& event, void* data)>;
    
    void SetLuaDispatcher(LuaDispatcher fn);
    void Update(float dt);
    void Draw();
    void SetState(const std::string& new_state);
    void Emit(const std::string& event, void* data = nullptr);
    const char* GetState();
}
