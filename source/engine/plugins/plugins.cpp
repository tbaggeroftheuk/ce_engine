#include <iostream>
#include <filesystem>
#include <string>

#include "globals.hpp"
#include "engine/plugins/plugins.hpp"

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

void LogError(const std::string message) {
    if (CE::debug) {
        std::cout << "ERROR: CE-Modules:" << message << "\n";
        return;
    }
    return;
}
// The top and below functions are needed due to raylib colliding with windows.h
void LogInfo(const std::string message) {
    if (CE::debug) {
        std::cout << "INFO: CE-Modules:" << message << "\n";
        return;
    }
    return;
}

static CE_Funcs funcs {
    .Version = 1,
    
    .Textures_Draw = CE::PluginAPI::Textures_Draw,
    .Textures_Unload = CE::PluginAPI::Textures_Unload,
    .Textures_UnloadAll = CE::PluginAPI::Textures_UnloadAll,

    .IsKeyPressed = CE::PluginAPI::IsKeyPressed,
    .IsKeyPressedRepeat = CE::PluginAPI::IsKeyPressedRepeat,
    .IsKeyDown = CE::PluginAPI::IsKeyDown,
    .IsKeyReleased = CE::PluginAPI::IsKeyReleased,
    .IsKeyUp = CE::PluginAPI::IsKeyUp,
    .GetKeyPressed = CE::PluginAPI::GetKeyPressed,
    .GetCharPressed = CE::PluginAPI::GetCharPressed,
};

static PluginGlobals g_PluginGlobals{};

namespace CE::Modules {
    void Init() {
        return;
    }

    void LoadModules() {
        namespace fs = std::filesystem;

        // Construct plugins folder path
        fs::path pluginsPath = fs::path(CE::Global.data_path) / "plugins";

        if (!fs::exists(pluginsPath)) {
            std::cout << "CE-Modules: Plugins folder does not exist, creating...\n";
            fs::create_directories(pluginsPath);
        }

        for (auto& entry : fs::directory_iterator(pluginsPath)) {

            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();

#ifdef _WIN32
            if (ext != ".dll") continue;
#else
            if (ext != ".so") continue;
#endif

            std::string fullPath = entry.path().string();
            std::cout << "CE-Modules: Loading plugin: " << fullPath << "\n";

            lib_handle lib = LOAD_LIB(fullPath.c_str());
            if (!lib) {
                std::cerr << "CE-Modules: Failed to load library: " << fullPath << "\n";
                continue;
            }

            auto GetInfo = (fn_GetPluginInfo)GET_SYM(lib, "CE_GetPluginInfo");
            auto InitFunc = (fn_PluginInit)GET_SYM(lib, "CE_PluginInit");
            auto UpdateFunc = (fn_PluginUpdate)GET_SYM(lib, "CE_PluginUpdate");
            auto ShutdownFunc = (fn_PluginShutdown)GET_SYM(lib, "CE_PluginShutdown");

            if (!GetInfo || !InitFunc) {
                std::cerr << "CE-Modules: Missing required plugin exports: " << fullPath << "\n";
                CLOSE_LIB(lib);
                continue;
            }

            CE_PluginInfo* info = GetInfo();

            if (!info || info->CE_Engine_Version_Major != 1) {
                std::cerr << "CE-Modules: Engine version mismatch in plugin: " << fullPath << "\n";
                CLOSE_LIB(lib);
                continue;
            }

            LoadedPlugin plugin{};
            plugin.handle = lib;
            plugin.info = info;
            plugin.Init = InitFunc;
            plugin.Update = UpdateFunc;
            plugin.Shutdown = ShutdownFunc;

            // Initialize plugin with engine functions + plugin globals
            plugin.Init(&funcs, &g_PluginGlobals);

            g_Plugins.push_back(plugin);

            std::cout << "CE-Modules: Loaded plugin: " << info->Name << "\n";
        }

        if (g_Plugins.empty()) {
            std::cout << "CE-Modules: No plugins found in " << pluginsPath << "\n";
        }
    }


    void Update() {
        for (auto& plugin : g_Plugins) {
            if (plugin.Update)
                plugin.Update();
        }
    }

    void Shutdown() {
        for (auto& plugin : g_Plugins) {
            if (plugin.Shutdown)
                plugin.Shutdown();

            CLOSE_LIB(plugin.handle);
        }

        g_Plugins.clear();
    }
}