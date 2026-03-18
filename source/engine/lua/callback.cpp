extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
    #include <raylib.h>
}

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/callbacks.hpp"
#include "engine/lua.hpp"

namespace {
    lua_State* g_L = nullptr;

    struct Key {
        std::string state;
        std::string event;
    };

    struct KeyHash {
        size_t operator()(const Key& k) const noexcept {
            std::hash<std::string> h;
            size_t a = h(k.state);
            size_t b = h(k.event);
            return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
        }
    };

    struct KeyEq {
        bool operator()(const Key& a, const Key& b) const noexcept {
            return a.state == b.state && a.event == b.event;
        }
    };

    struct CallbackRec {
        int id{};
        int ref{};
        bool once{};
    };

    static std::unordered_map<Key, std::vector<CallbackRec>, KeyHash, KeyEq> g_LuaCallbacks;
    static std::unordered_map<int, std::pair<Key, int>> g_IdToKeyAndIndex; // id -> (key, index-in-vector)
    static int g_NextCallbackId = 1;

    static std::string_view NormalizeState(const char* stateName) {
        if (!stateName || !*stateName) return "*";
        if (stateName[0] == '*' && stateName[1] == '\0') return "*";
        return stateName;
    }

    static void UnregisterById(int id) {
        auto it = g_IdToKeyAndIndex.find(id);
        if (it == g_IdToKeyAndIndex.end()) return;

        const Key key = it->second.first;
        const int idx = it->second.second;

        auto vit = g_LuaCallbacks.find(key);
        if (vit == g_LuaCallbacks.end() || idx < 0 || idx >= (int)vit->second.size()) {
            g_IdToKeyAndIndex.erase(it);
            return;
        }

        const int ref = vit->second[(size_t)idx].ref;
        luaL_unref(g_L, LUA_REGISTRYINDEX, ref);

        const int lastIndex = (int)vit->second.size() - 1;
        if (idx != lastIndex) {
            vit->second[(size_t)idx] = vit->second[(size_t)lastIndex];
            const int movedId = vit->second[(size_t)idx].id;
            auto movedIt = g_IdToKeyAndIndex.find(movedId);
            if (movedIt != g_IdToKeyAndIndex.end()) {
                movedIt->second.second = idx;
            }
        }
        vit->second.pop_back();
        if (vit->second.empty()) g_LuaCallbacks.erase(vit);

        g_IdToKeyAndIndex.erase(it);
    }

    static void CallList(std::string_view state, std::string_view event, float* dtOrNull) {
        if (!g_L) return;

        auto it = g_LuaCallbacks.find(Key{std::string(state), std::string(event)});
        if (it == g_LuaCallbacks.end()) return;

        std::vector<int> onceIds;
        for (const auto& rec : it->second) {
            lua_rawgeti(g_L, LUA_REGISTRYINDEX, rec.ref);
            lua_pushstring(g_L, state.data());
            lua_pushstring(g_L, event.data());
            if (dtOrNull) lua_pushnumber(g_L, (lua_Number)*dtOrNull);
            else lua_pushnil(g_L);

            if (lua_pcall(g_L, 3, 0, 0) != LUA_OK) {
                const char* err = lua_tostring(g_L, -1);
                TraceLog(LOG_ERROR, "CE-LuaCallbacks: %s callback error: %s", event.data(), err ? err : "<unknown>");
                lua_pop(g_L, 1);
            }

            if (rec.once) onceIds.push_back(rec.id);
        }

        for (int id : onceIds) UnregisterById(id);
    }

    static void DispatchFromEngine(const std::string& event, void* data) {
        float* dt = nullptr;
        float dtValue = 0.0f;
        if (data && event == "Update") {
            dtValue = *(float*)data;
            dt = &dtValue;
        }

        const char* state = CE::Callbacks::GetState();
        const std::string_view stateView = (state && *state) ? std::string_view(state) : std::string_view("None");

        CallList("*", event, dt);
        CallList(stateView, event, dt);
    }

    static int Lua_Callbacks_OnInternal(lua_State* L, bool once) {
        const int argc = lua_gettop(L);

        const char* stateName = "*";
        const char* eventName = nullptr;
        int fnIndex = 0;

        if (argc == 2) {
            eventName = luaL_checkstring(L, 1);
            fnIndex = 2;
        } else if (argc == 3) {
            stateName = luaL_checkstring(L, 1);
            eventName = luaL_checkstring(L, 2);
            fnIndex = 3;
        } else {
            return luaL_error(L, "Callbacks.On expects (event, fn) or (state, event, fn)");
        }

        luaL_checktype(L, fnIndex, LUA_TFUNCTION);
        lua_pushvalue(L, fnIndex);
        const int ref = luaL_ref(L, LUA_REGISTRYINDEX);

        std::string_view stateView = NormalizeState(stateName);
        std::string_view eventView = eventName ? std::string_view(eventName) : std::string_view("");

        Key key{std::string(stateView), std::string(eventView)};
        const int id = g_NextCallbackId++;
        auto& vec = g_LuaCallbacks[key];
        vec.push_back(CallbackRec{ .id = id, .ref = ref, .once = once });
        g_IdToKeyAndIndex[id] = std::make_pair(key, (int)vec.size() - 1);

        lua_pushinteger(L, id);
        return 1;
    }

    static int Lua_Callbacks_On(lua_State* L) { return Lua_Callbacks_OnInternal(L, false); }
    static int Lua_Callbacks_Once(lua_State* L) { return Lua_Callbacks_OnInternal(L, true); }

    static int Lua_Callbacks_Off(lua_State* L) {
        const int id = (int)luaL_checkinteger(L, 1);
        const bool exists = (g_IdToKeyAndIndex.find(id) != g_IdToKeyAndIndex.end());
        if (exists) UnregisterById(id);
        lua_pushboolean(L, exists ? 1 : 0);
        return 1;
    }

    static int Lua_Callbacks_Clear(lua_State* L) {
        (void)L;
        if (!g_L) return 0;

        for (auto& [key, refs] : g_LuaCallbacks) {
            for (const auto& rec : refs) {
                luaL_unref(g_L, LUA_REGISTRYINDEX, rec.ref);
            }
        }
        g_LuaCallbacks.clear();
        g_IdToKeyAndIndex.clear();
        return 0;
    }

    static int Lua_Callbacks_Emit(lua_State* L) {
        const int argc = lua_gettop(L);
        const char* event = luaL_checkstring(L, 1);

        if (argc >= 2 && lua_isnumber(L, 2)) {
            float temp = (float)lua_tonumber(L, 2);
            CE::Callbacks::Emit(std::string(event), &temp);
            return 0;
        }

        CE::Callbacks::Emit(std::string(event), nullptr);
        return 0;
    }

    static int Lua_Callbacks_GetState(lua_State* L) {
        lua_pushstring(L, CE::Callbacks::GetState());
        return 1;
    }

    static int Lua_Callbacks_SetState(lua_State* L) {
        const char* state = luaL_checkstring(L, 1);
        CE::Callbacks::SetState(std::string(state));
        return 0;
    }

    static int Lua_Callbacks_OnEvent(lua_State* L, const char* eventName, bool once) {
        const int argc = lua_gettop(L);

        const char* stateName = "*";
        int fnIndex = 0;

        if (argc == 1) {
            fnIndex = 1;
        } else if (argc == 2) {
            stateName = luaL_checkstring(L, 1);
            fnIndex = 2;
        } else {
            return luaL_error(L, "Expected (fn) or (state, fn)");
        }

        luaL_checktype(L, fnIndex, LUA_TFUNCTION);
        lua_pushvalue(L, fnIndex);
        const int ref = luaL_ref(L, LUA_REGISTRYINDEX);

        std::string_view stateView = NormalizeState(stateName);
        Key key{std::string(stateView), std::string(eventName)};

        const int id = g_NextCallbackId++;
        auto& vec = g_LuaCallbacks[key];
        vec.push_back(CallbackRec{ .id = id, .ref = ref, .once = once });
        g_IdToKeyAndIndex[id] = std::make_pair(key, (int)vec.size() - 1);

        lua_pushinteger(L, id);
        return 1;
    }

    static int Lua_Callbacks_OnUpdate(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Update", false); }
    static int Lua_Callbacks_OnDraw(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Draw", false); }
    static int Lua_Callbacks_OnEnter(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Enter", false); }
    static int Lua_Callbacks_OnExit(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Exit", false); }

    static int Lua_Callbacks_OnceUpdate(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Update", true); }
    static int Lua_Callbacks_OnceDraw(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Draw", true); }
    static int Lua_Callbacks_OnceEnter(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Enter", true); }
    static int Lua_Callbacks_OnceExit(lua_State* L) { return Lua_Callbacks_OnEvent(L, "Exit", true); }
}

namespace CE::Lua::Functions::Callbacks {
    void Register(lua_State* L) {
        g_L = L;

        CE::Callbacks::SetLuaDispatcher([](const std::string& event, void* data) {
            DispatchFromEngine(event, data);
        });

        lua_newtable(L); // Callbacks table

        lua_pushcfunction(L, Lua_Callbacks_On);
        lua_setfield(L, -2, "On");
        lua_pushcfunction(L, Lua_Callbacks_Once);
        lua_setfield(L, -2, "Once");
        lua_pushcfunction(L, Lua_Callbacks_Off);
        lua_setfield(L, -2, "Off");
        lua_pushcfunction(L, Lua_Callbacks_Clear);
        lua_setfield(L, -2, "Clear");
        lua_pushcfunction(L, Lua_Callbacks_Emit);
        lua_setfield(L, -2, "Emit");
        lua_pushcfunction(L, Lua_Callbacks_GetState);
        lua_setfield(L, -2, "GetState");
        lua_pushcfunction(L, Lua_Callbacks_SetState);
        lua_setfield(L, -2, "SetState");

        lua_pushcfunction(L, Lua_Callbacks_OnUpdate);
        lua_setfield(L, -2, "OnUpdate");
        lua_pushcfunction(L, Lua_Callbacks_OnDraw);
        lua_setfield(L, -2, "OnDraw");
        lua_pushcfunction(L, Lua_Callbacks_OnEnter);
        lua_setfield(L, -2, "OnEnter");
        lua_pushcfunction(L, Lua_Callbacks_OnExit);
        lua_setfield(L, -2, "OnExit");

        lua_pushcfunction(L, Lua_Callbacks_OnceUpdate);
        lua_setfield(L, -2, "OnceUpdate");
        lua_pushcfunction(L, Lua_Callbacks_OnceDraw);
        lua_setfield(L, -2, "OnceDraw");
        lua_pushcfunction(L, Lua_Callbacks_OnceEnter);
        lua_setfield(L, -2, "OnceEnter");
        lua_pushcfunction(L, Lua_Callbacks_OnceExit);
        lua_setfield(L, -2, "OnceExit");

        // Lowercase aliases
        lua_getfield(L, -1, "On");
        lua_setfield(L, -2, "on");
        lua_getfield(L, -1, "Once");
        lua_setfield(L, -2, "once");
        lua_getfield(L, -1, "Off");
        lua_setfield(L, -2, "off");
        lua_getfield(L, -1, "Emit");
        lua_setfield(L, -2, "emit");
        lua_getfield(L, -1, "GetState");
        lua_setfield(L, -2, "state");

        lua_setglobal(L, "Callbacks");
    }
}
