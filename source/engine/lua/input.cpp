extern "C" {
    #include <raylib.h>
}

#include <string>
#include <string_view>
#include <unordered_map>
#include <cctype>

#define LCS luaL_checkstring
#define LCI luaL_checkinteger
#define LCN luaL_checknumber
#define LPB lua_pushboolean
#define LPS lua_pushstring
#define LPN lua_pushnumber

#include "engine/lua.hpp"
#include "globals.hpp"

int CE_HideCursor(lua_State* L) {
    HideCursor();
    return 0;
}

int CE_ShowCursor(lua_State* L) {
    ShowCursor();
    return 0;
}

int CE_EnableCursor(lua_State* L) {
    EnableCursor();
    return 0;
}

int CE_DisableCursor(lua_State* L) {
    DisableCursor();
    return 0;
}

int CE_IsCursorHidden(lua_State* L) {
    lua_pushboolean(L, IsCursorHidden());
    return 1;
}

int CE_IsCursorOnScreen(lua_State* L) {
    lua_pushboolean(L, IsCursorOnScreen());
    return 1;
}

static std::string NormalizeKeyName(std::string_view key) {
    std::string out;
    out.reserve(key.size());

    char prev = '\0';
    for (unsigned char c : key) {
        if (std::isalnum(c)) {
            out.push_back((char)std::toupper(c));
            prev = out.back();
            continue;
        }

        if (c == ' ' || c == '-' || c == '_') {
            if (prev != '_') {
                out.push_back('_');
                prev = '_';
            }
            continue;
        }
    }

    while (!out.empty() && out.front() == '_') out.erase(out.begin());
    while (!out.empty() && out.back() == '_') out.pop_back();
    return out;
}

static bool TryKeyFromString(std::string_view keyName, int* outKey) {
    if (!outKey) return false;

    std::string key = NormalizeKeyName(keyName);
    if (key.empty()) return false;
    if (key.size() == 1) {
        char c = key[0];
        if (c >= 'A' && c <= 'Z') {
            *outKey = KEY_A + (c - 'A');
            return true;
        }
        if (c >= '0' && c <= '9') {
            *outKey = KEY_ZERO + (c - '0');
            return true;
        }
    }
    if (key.size() >= 2 && key[0] == 'F') {
        int n = 0;
        for (size_t i = 1; i < key.size(); i++) {
            if (key[i] < '0' || key[i] > '9') { n = 0; break; }
            n = (n * 10) + (key[i] - '0');
        }
        if (n >= 1 && n <= 24) {
            *outKey = KEY_F1 + (n - 1);
            return true;
        }
    }
    if (key.rfind("KP", 0) == 0 || key.rfind("NUMPAD", 0) == 0) {
        std::string digits;
        digits.reserve(2);
        for (unsigned char c : key) {
            if (c >= '0' && c <= '9') digits.push_back((char)c);
        }
        if (digits.size() == 1) {
            *outKey = KEY_KP_0 + (digits[0] - '0');
            return true;
        }
    }

    static const std::unordered_map<std::string, int> map = {
        {"SPACE", KEY_SPACE},
        {"ENTER", KEY_ENTER},
        {"RETURN", KEY_ENTER},
        {"TAB", KEY_TAB},
        {"BACKSPACE", KEY_BACKSPACE},
        {"ESC", KEY_ESCAPE},
        {"ESCAPE", KEY_ESCAPE},
        {"LEFT", KEY_LEFT},
        {"RIGHT", KEY_RIGHT},
        {"UP", KEY_UP},
        {"DOWN", KEY_DOWN},
        {"HOME", KEY_HOME},
        {"END", KEY_END},
        {"PAGE_UP", KEY_PAGE_UP},
        {"PAGE_DOWN", KEY_PAGE_DOWN},
        {"INSERT", KEY_INSERT},
        {"DELETE", KEY_DELETE},
        {"SHIFT", KEY_LEFT_SHIFT},
        {"LEFT_SHIFT", KEY_LEFT_SHIFT},
        {"RIGHT_SHIFT", KEY_RIGHT_SHIFT},
        {"CTRL", KEY_LEFT_CONTROL},
        {"CONTROL", KEY_LEFT_CONTROL},
        {"LEFT_CONTROL", KEY_LEFT_CONTROL},
        {"RIGHT_CONTROL", KEY_RIGHT_CONTROL},
        {"ALT", KEY_LEFT_ALT},
        {"LEFT_ALT", KEY_LEFT_ALT},
        {"RIGHT_ALT", KEY_RIGHT_ALT},
        {"SUPER", KEY_LEFT_SUPER},
        {"LEFT_SUPER", KEY_LEFT_SUPER},
        {"RIGHT_SUPER", KEY_RIGHT_SUPER},
        {"CAPS_LOCK", KEY_CAPS_LOCK},
        {"NUM_LOCK", KEY_NUM_LOCK},
        {"SCROLL_LOCK", KEY_SCROLL_LOCK},
        {"PAUSE", KEY_PAUSE},
        {"PRINT_SCREEN", KEY_PRINT_SCREEN},
        {"MENU", KEY_MENU},
        {"APOSTROPHE", KEY_APOSTROPHE},
        {"QUOTE", KEY_APOSTROPHE},
        {"COMMA", KEY_COMMA},
        {"PERIOD", KEY_PERIOD},
        {"DOT", KEY_PERIOD},
        {"SLASH", KEY_SLASH},
        {"SEMICOLON", KEY_SEMICOLON},
        {"EQUAL", KEY_EQUAL},
        {"MINUS", KEY_MINUS},
        {"LEFT_BRACKET", KEY_LEFT_BRACKET},
        {"RIGHT_BRACKET", KEY_RIGHT_BRACKET},
        {"BACKSLASH", KEY_BACKSLASH},
        {"GRAVE", KEY_GRAVE},
        {"BACKTICK", KEY_GRAVE},
        {"KP_DECIMAL", KEY_KP_DECIMAL},
        {"KP_DIVIDE", KEY_KP_DIVIDE},
        {"KP_MULTIPLY", KEY_KP_MULTIPLY},
        {"KP_SUBTRACT", KEY_KP_SUBTRACT},
        {"KP_ADD", KEY_KP_ADD},
        {"KP_ENTER", KEY_KP_ENTER},
        {"KP_EQUAL", KEY_KP_EQUAL},
    };

    auto it = map.find(key);
    if (it == map.end()) return false;
    *outKey = it->second;
    return true;
}

int CE_Input_IsKeyDown(lua_State* L) {
    const char* keyName = LCS(L, 1);
    int key = 0;
    if (!TryKeyFromString(keyName, &key)) {
        return luaL_error(L, "Input.IsKeyDown: unknown key '%s'", keyName);
    }

    lua_pushboolean(L, IsKeyDown(key));
    return 1;
}

int CE_Input_IsKeyReleased(lua_State* L) {
    const char* keyName = LCS(L, 1);
    int key = 0;
    if (!TryKeyFromString(keyName, &key)) {
        return luaL_error(L, "Input.IsKeyDown: unknown key '%s'", keyName);
    }

    lua_pushboolean(L, IsKeyReleased(key));
    return 1;
}

int CE_Input_IsKeyPressed(lua_State* L) {
    const char* keyName = LCS(L, 1);
    int key = 0;
    if (!TryKeyFromString(keyName, &key)) {
        return luaL_error(L, "Input.IsKeyDown: unknown key '%s'", keyName);
    }

    lua_pushboolean(L, IsKeyPressed(key));
    return 1;
}

int CE_Input_IsKeyPressedRepeat(lua_State* L) {
    const char* keyName = LCS(L, 1);
    int key = 0;
    if (!TryKeyFromString(keyName, &key)) {
        return luaL_error(L, "Input.IsKeyDown: unknown key '%s'", keyName);
    }

    lua_pushboolean(L, IsKeyPressedRepeat(key));
    return 1;
}

int CE_Input_IsKeyUp(lua_State* L) {
    const char* keyName = LCS(L, 1);
    int key = 0;
    if (!TryKeyFromString(keyName, &key)) {
        return luaL_error(L, "Input.IsKeyDown: unknown key '%s'", keyName);
    }

    lua_pushboolean(L, IsKeyUp(key));
    return 1;
}

int CE_Mouse_GetPosition(lua_State* L) {
    Vector2& pos = CE::MousePos;
    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    return 2;
}

namespace CE::Lua::Functions::RaylibBindings {
    void Register(lua_State* L) {
        lua_newtable(L); // Table [Input]

        lua_pushcfunction(L, CE_Input_IsKeyPressed);
        lua_setfield(L, -2, "IsKeyPressed");

        lua_pushcfunction(L, CE_Input_IsKeyPressedRepeat);
        lua_setfield(L, -2, "IsKeyPressedRepeat");

        lua_pushcfunction(L, CE_Input_IsKeyDown);
        lua_setfield(L, -2, "IsKeyDown");

        lua_pushcfunction(L, CE_Input_IsKeyReleased);
        lua_setfield(L, -2, "IsKeyReleased");

        lua_pushcfunction(L, CE_Input_IsKeyUp);
        lua_setfield(L, -2, "IsKeyUp");

        lua_setglobal(L, "Input");

        lua_newtable(L); // Table [Mouse]

        lua_pushcfunction(L, CE_HideCursor);
        lua_setfield(L, -2, "Hide");

        lua_pushcfunction(L, CE_ShowCursor);
        lua_setfield(L, -2, "Show");

        lua_pushcfunction(L, CE_EnableCursor);
        lua_setfield(L, -2, "Enable");

        lua_pushcfunction(L, CE_DisableCursor);
        lua_setfield(L, -2, "Disable");

        lua_pushcfunction(L, CE_IsCursorHidden);
        lua_setfield(L, -2, "IsCursorHidden");

        lua_pushcfunction(L, CE_IsCursorOnScreen);
        lua_setfield(L, -2, "IsCursorOnScreen");

        lua_pushcfunction(L, CE_Mouse_GetPosition);
        lua_setfield(L, -2, "GetMousePosition");

        lua_setglobal(L, "Mouse");
    }   
}
