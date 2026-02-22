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

struct CE_Funcs {
    uint32_t Version;

    // Texture functions
    void (*Textures_Draw)(const char* path, uint32_t posX, uint32_t posY);
    void (*Textures_Unload)(const char* name);
    void (*Textures_UnloadAll)(void);

    // Keyboard functions
    uint32_t (*IsKeyPressed)(uint32_t key);
    uint32_t (*IsKeyPressedRepeat)(uint32_t key);
    uint32_t (*IsKeyDown)(uint32_t key);
    uint32_t (*IsKeyReleased)(uint32_t key);
    uint32_t (*IsKeyUp)(uint32_t key);
    uint32_t (*GetKeyPressed)(void);
    uint32_t (*GetCharPressed)(void);
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
    CE_UI_Pause_Menu = 1 << 4
};

typedef CE_PluginInfo* (*fn_GetPluginInfo)();
typedef void (*fn_PluginInit)(CE_Funcs*, PluginGlobals*);
typedef void (*fn_PluginUpdate)();
typedef void (*fn_PluginShutdown)();

namespace CE::Modules {
    void Init();
    void LoadModules();
    void Update();
    void Shutdown();

    struct LoadedPlugin {
        lib_handle handle{};
        CE_PluginInfo* info{};

        fn_PluginInit Init{};
        fn_PluginUpdate Update{};
        fn_PluginShutdown Shutdown{};
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
    void Textures_Draw(const char* path, uint32_t posX, uint32_t posY);
    void Textures_Unload(const char* name);
    void Textures_UnloadAll();

    // Keyboard
    uint32_t IsKeyPressed(uint32_t key);
    uint32_t IsKeyPressedRepeat(uint32_t key);
    uint32_t IsKeyDown(uint32_t key);
    uint32_t IsKeyReleased(uint32_t key);
    uint32_t IsKeyUp(uint32_t key);
    uint32_t GetKeyPressed(void);
    uint32_t GetCharPressed(void);
}