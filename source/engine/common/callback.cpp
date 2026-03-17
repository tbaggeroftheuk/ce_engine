#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <cstdint>

#include "engine/callbacks.hpp"

namespace CE::Callbacks {

    using HashID = uint64_t;
    using Callback0 = CE::Callbacks::Callback0;
    using Callback1 = CE::Callbacks::Callback1;

    struct Key {
        HashID state;
        HashID event;
    };

    struct KeyHash {
        size_t operator()(const Key& k) const noexcept {
            HashID x = k.state ^ (k.event + 0x9e3779b97f4a7c15ULL + (k.state << 6) + (k.state >> 2));
            x ^= x >> 30;
            x *= 0xbf58476d1ce4e5b9ULL;
            x ^= x >> 27;
            x *= 0x94d049bb133111ebULL;
            x ^= x >> 31;
            return (size_t)x;
        }
    };

    struct KeyEq {
        bool operator()(const Key& a, const Key& b) const noexcept {
            return a.state == b.state && a.event == b.event;
        }
    };

    struct CallbackEntry {
        Callback0 fn0{};
        Callback1 fn1{};
        bool withData{};
    };

    static std::unordered_map<Key, std::vector<CallbackEntry>, KeyHash, KeyEq> g_Callbacks;
    static std::string g_CurrentState = "None";

    static HashID Hash(std::string_view s) {
        HashID hash = 1469598103934665603ULL;
        for (unsigned char c : s) {
            hash ^= c;
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    static std::string_view NormalizeState(const char* stateName) {
        if (!stateName || !*stateName) return "*";
        if (stateName[0] == '*' && stateName[1] == '\0') return "*";
        return stateName;
    }

    static void Dispatch(HashID stateHash, HashID eventHash, void* data) {
        auto it = g_Callbacks.find(Key{stateHash, eventHash});
        if (it == g_Callbacks.end()) return;

        const auto& callbacks = it->second;

        for (const auto& entry : callbacks) {
            if (entry.withData) {
                if (entry.fn1) entry.fn1(data);
                continue;
            }
            if (entry.fn0) entry.fn0();
        }
    }

    void SetGameState(const char* stateName) {
        std::string_view state = NormalizeState(stateName);

        if (state == "*") state = "None"; 

        g_CurrentState.assign(state.begin(), state.end());
    }

    static const char* GameStateToString(CE::GameState state) {
        switch (state) {
            case CE::GameState::None: return "None";
            case CE::GameState::MainMenu: return "MainMenu";
            case CE::GameState::PauseMenu: return "PauseMenu";
            case CE::GameState::InGame: return "InGame";
        }
        return "None";
    }

    void SetGameState(CE::GameState state) {
        SetGameState(GameStateToString(state));
    }

    const char* GetGameState() {
        return g_CurrentState.c_str();
    }

    void Register(const char* stateName, const char* eventName, Callback0 fn) {
        if (!eventName || !*eventName || !fn) return;

        std::string_view state = NormalizeState(stateName);

        HashID stateHash = (state == "*") ? 0 : Hash(state);
        HashID eventHash = Hash(eventName);

        g_Callbacks[Key{stateHash, eventHash}].push_back(CallbackEntry{ .fn0 = fn, .withData = false });
    }

    void Register(const char* stateName, const char* eventName, Callback1 fn) {
        if (!eventName || !*eventName || !fn) return;

        std::string_view state = NormalizeState(stateName);

        HashID stateHash = (state == "*") ? 0 : Hash(state);
        HashID eventHash = Hash(eventName);

        g_Callbacks[Key{stateHash, eventHash}].push_back(CallbackEntry{ .fn1 = fn, .withData = true });
    }

    void Emit(const char* stateName, const char* eventName, void* data) {
        if (!eventName || !*eventName) return;

        std::string_view state = NormalizeState(stateName);

        HashID stateHash = (state == "*") ? 0 : Hash(state);
        HashID eventHash = Hash(eventName);
        Dispatch(0, eventHash, data);
        if (stateHash != 0) {
            Dispatch(stateHash, eventHash, data);
        }
    }

    void Emit(const char* eventName, void* data) {
        Emit(g_CurrentState.c_str(), eventName, data);
    }

    void Clear() {
        g_Callbacks.clear();
    }

}
