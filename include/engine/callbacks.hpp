#pragma once

#include <cstdint>

#include "globals.hpp"

namespace CE::Callbacks {
    using Callback0 = void(*)();
    using Callback1 = void(*)(void*);

    void SetGameState(const char* stateName);
    void SetGameState(CE::GameState state);
    const char* GetGameState();

    void Register(const char* stateName, const char* eventName, Callback0 fn);
    void Register(const char* stateName, const char* eventName, Callback1 fn);

    void Emit(const char* eventName, void* data = nullptr);            
    void Emit(const char* stateName, const char* eventName, void* data); 
    void Clear();
}
