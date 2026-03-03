#pragma once

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
    #include <raylib.h>
}

#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIB(name) LoadLibraryA(name)
    #define GET_SYM(lib, name) GetProcAddress(lib, name)
    #define CLOSE_LIB(lib) FreeLibrary(lib)
    typedef HMODULE lib_handle;
#else
    #include <dlfcn.h>
    #define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
    #define GET_SYM(lib, name) dlsym(lib, name)
    #define CLOSE_LIB(lib) dlclose(lib)
    typedef void* lib_handle;
#endif

#define CE_LOG_INFO ((uint32_t)1)
#define CE_LOG_DEBUG ((uint32_t)2)
#define CE_LOG_WARN ((uint32_t)3)
#define CE_LOG_ERROR ((uint32_t)4)

struct CE_Funcs {
    uint32_t Version;

    void (*TexturesDraw)(const char* path, uint32_t posX, uint32_t posY);
    void (*TexturesUnload)(const char* name);
    void (*TexturesUnloadAll)(void);

    uint32_t (*CeIsKeyPressed)(uint32_t key);
    uint32_t (*CeIsKeyPressedRepeat)(uint32_t key);
    uint32_t (*CeIsKeyDown)(uint32_t key);
    uint32_t (*CeIsKeyReleased)(uint32_t key);
    uint32_t (*CeIsKeyUp)(uint32_t key);
    uint32_t (*CeGetKeyPressed)(void);
    uint32_t (*CeGetCharPressed)(void);

    void (*Log)(const uint32_t log_level, const char* format, ...);
};

typedef struct CE_PluginInfo {
    uint32_t StructSize;

    uint32_t Plugin_Version_Major;
    uint32_t Plugin_Version_Minor;

    uint32_t CE_Engine_Version_Major;
    uint32_t CE_Engine_version_Minor;

    const char* Name;
    const char* Description;

    uint32_t Capabilities;

} CE_PluginInfo;

struct PluginGlobals {
    uint32_t test;
};

enum CE_Capabilities : uint32_t {
    CE_Lua_Injects   = 1 << 0,
    CE_InGame        = 1 << 1,
    CE_UI_Main_Menu  = 1 << 2,
    CE_UI_InGame     = 1 << 3,
    CE_UI_Pause_Menu = 1 << 4,
};

typedef CE_PluginInfo* (*fn_GetPluginInfo)();
typedef void (*fn_PluginInit)(CE_Funcs*, PluginGlobals*);
typedef void (*fn_PluginUpdate)();
typedef void (*fn_PluginShutdown)();

// Lua injecting
typedef void (*fn_PluginLuaInject)();

// Ingame update
typedef void (*fn_PluginInGameUpdate)();

// UI stuff
typedef void (*fn_PluginMainMenuUpdateUI)();
typedef void (*fn_PluginPauseMenuUpdateUI)();
typedef void (*fn_PluginInGameUpdateUI)();

namespace CE::Plugins {
    void Init();
    void LoadModules();
    void Update();
    void UpdateInGame();

    void UpdateMainMenuUI();
    void UpdateInGameUI();
    void UpdatePauseMenuUI();
    void Shutdown();

    struct LoadedPlugin {
        lib_handle handle{};
        CE_PluginInfo* info{};

        fn_PluginInit Init{};
        fn_PluginShutdown Shutdown{};
        fn_PluginUpdate Update{};

        fn_PluginLuaInject LuaInject{};

        fn_PluginInGameUpdate InGameUpdate{};

        fn_PluginPauseMenuUpdateUI UpdateUI_PauseMenu{};
        fn_PluginMainMenuUpdateUI UpdateUI_MainMenu{};
        fn_PluginInGameUpdate UpdateUI_InGameMenu{};
    };

    static std::vector<LoadedPlugin> g_Plugins;

}

namespace CE::Assets::Textures {
    void Draw(const std::string& path, uint32_t posX, uint32_t posY);
    void Unload(const std::string& name);
    void UnloadAll();
}

namespace CE::PluginAPI {

    // Texture
    void TexturesDraw(const char* path, uint32_t posX, uint32_t posY);
    void TexturesUnload(const char* name);
    void TexturesUnloadAll();

    // Logging (implemented internally)
    void Log(const uint32_t log_level, const char* format, ...);

    // Keyboard helpers (CE prefix to avoid naming conflicts with raylib)
    uint32_t CeIsKeyPressed(uint32_t key);
    uint32_t CeIsKeyPressedRepeat(uint32_t key);
    uint32_t CeIsKeyDown(uint32_t key);
    uint32_t CeIsKeyReleased(uint32_t key);
    uint32_t CeIsKeyUp(uint32_t key);
    uint32_t CeGetKeyPressed(void);
    uint32_t CeGetCharPressed(void);
}