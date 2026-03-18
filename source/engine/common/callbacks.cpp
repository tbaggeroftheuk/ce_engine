#include <string>
#include <functional>
#include <unordered_map>
#include "engine/callbacks.hpp"

namespace CE::Callbacks {
    using LuaDispatcher = std::function<void(const std::string& event, void* data)>;

    struct State {
        std::string current_state = "None";
        LuaDispatcher lua_dispatcher;
    } g_state;

    void SetLuaDispatcher(LuaDispatcher fn) {
        g_state.lua_dispatcher = fn;
    }

    void Update(float dt) {
        if (g_state.lua_dispatcher) 
            g_state.lua_dispatcher("Update", &dt);
    }

    void Draw() {
        if (g_state.lua_dispatcher) 
            g_state.lua_dispatcher("Draw", nullptr);
    }

    void SetState(const std::string& new_state) {
        std::string next = new_state;
        if (next.empty()) next = "None";

        if (next == g_state.current_state) return;

        // Emit Exit while we're still in the old state so state-filtered handlers work.
        if (g_state.lua_dispatcher && !g_state.current_state.empty() && g_state.current_state != "None") {
            g_state.lua_dispatcher("Exit", nullptr);
        }

        g_state.current_state = next;

        if (g_state.lua_dispatcher) {
            g_state.lua_dispatcher("Enter", nullptr);
            // Also emit a state-name event (e.g. "InGame") for convenience.
            g_state.lua_dispatcher(g_state.current_state, nullptr);
        }
    }

    void Emit(const std::string& event, void* data) {
        if (g_state.lua_dispatcher) 
            g_state.lua_dispatcher(event, data);
    }

    const char* GetState() {
        return g_state.current_state.c_str();
    }

}
