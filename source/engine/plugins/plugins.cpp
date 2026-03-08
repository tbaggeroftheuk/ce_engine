#include <iostream>
#include <filesystem>
#include <string>

#include <cstdint>
#include "globals.hpp"
#include "engine/plugins/plugins.hpp"

struct CapabilityEntry {
    uint32_t flag;
    const char* name;
};

static const CapabilityEntry g_CapabilityTable[] = {
    { CE_Lua_Injects,   "Lua Injects" },
    { CE_InGame,        "In Game" },
    { CE_UI_Main_Menu,  "UI Main Menu" },
    { CE_UI_InGame,     "UI In Game" },
    { CE_UI_Pause_Menu, "UI Pause Menu" },
};

void LogError(const std::string message) {
    if (CE::Debug) {
        std::cout << "ERROR: CE-PluginLoader:" << message << "\n";
        return;
    }
    return;
}
// The top and below functions are needed due to raylib colliding with windows.h
void LogInfo(const std::string message) {
    if (CE::Debug) {
        std::cout << "INFO: CE-PluginLoader:" << message << "\n";
        return;
    }
    return;
}

static CE_Funcs Funcs {
    .Version = 1,
    
    .TexturesDraw = CE::PluginAPI::TexturesDraw,
    .TexturesUnload = CE::PluginAPI::TexturesUnload,
    .TexturesUnloadAll = CE::PluginAPI::TexturesUnloadAll,

    .CeIsKeyPressed = CE::PluginAPI::CeIsKeyPressed,
    .CeIsKeyPressedRepeat = CE::PluginAPI::CeIsKeyPressedRepeat,
    .CeIsKeyDown = CE::PluginAPI::CeIsKeyDown,
    .CeIsKeyReleased = CE::PluginAPI::CeIsKeyReleased,
    .CeIsKeyUp = CE::PluginAPI::CeIsKeyUp,
    .CeGetKeyPressed = CE::PluginAPI::CeGetKeyPressed,
    .CeGetCharPressed = CE::PluginAPI::CeGetCharPressed,

    .Log = CE::PluginAPI::Log,
};

static PluginGlobals GPluginGlobals{};

namespace CE::Plugins {
    void Init() {
        return;
    }

    void LoadModules() {
        namespace fs = std::filesystem;

        fs::path PluginsPath = fs::path(CE::Global.data_path) / "plugins";

        if (!fs::exists(PluginsPath)) {
            LogInfo("CE-PluginLoader: Plugins folder does not exist! If you don't have any plugins you can ignore this message");
            return;
        }

        for (auto& Entry : fs::directory_iterator(PluginsPath)) {

            if (!Entry.is_regular_file()) continue;

            std::string Ext = Entry.path().extension().string();

            #if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) // I bless apon you, CROSS PLATFORM!
                        if (Ext != ".so") continue;
            #elif defined(__APPLE__)
                        if (Ext != ".dylib") continue;
            #elif defined(_WIN32)
                        if (Ext != ".dll") continue;
            #else
                        continue;
            #endif

            std::string FullPath = Entry.path().string();
            std::cout << "INFO: CE-PluginLoader: Loading plugin: " << FullPath << "\n";

            lib_handle Lib = LOAD_LIB(FullPath.c_str());
            if (!Lib) {
                std::cerr << "ERROR: CE-PluginLoader: Failed to load library: " << FullPath << "\n";
                continue;
            }

            auto GetInfo = (fn_GetPluginInfo)GET_SYM(Lib, "CE_GetPluginInfo");
            auto InitFunc = (fn_PluginInit)GET_SYM(Lib, "CE_PluginInit");
            auto UpdateFunc = (fn_PluginUpdate)GET_SYM(Lib, "CE_PluginUpdate");
            auto ShutdownFunc = (fn_PluginShutdown)GET_SYM(Lib, "CE_PluginShutdown");

            auto LuaFuncReg = (fn_PluginLuaInject)GET_SYM(Lib, "CE_PluginLuaFuncReg");
            auto InGameUpdate = (fn_PluginInGameUpdate)GET_SYM(Lib, "CE_PluginInGameUpdate");

            auto UpdateMainMenuUI = (fn_PluginMainMenuUpdateUI)GET_SYM(Lib, "CE_PluginUpdateMainMenuUI");
            auto UpdateInGameUI = (fn_PluginInGameUpdateUI)GET_SYM(Lib, "CE_PluginUpdateInGameUI");
            auto UpdatePauseMenuUI = (fn_PluginPauseMenuUpdateUI)GET_SYM(Lib, "CE_PluginUpdatePauseMenuUI");

            if (!GetInfo || !InitFunc || !UpdateFunc || !ShutdownFunc) {
                std::cerr << "INFO: CE-PluginLoader: Missing required plugin exports!" << FullPath << "\n";
                std::cerr << "INFO: CE-PluginLoader: Required Exports are\n";
                CLOSE_LIB(Lib);
                continue;
            }

            CE_PluginInfo* Info = GetInfo();

            if (!Info || Info->CE_Engine_Version_Major != static_cast<uint32_t>(CE::int_engine_ver)) {
                std::cerr << "INFO: CE-PluginLoader: Engine version mismatch in plugin: " << FullPath << "\n";
                CLOSE_LIB(Lib);
                continue;
            }

            if ((Info && (Info->Capabilities & CE_UI_Main_Menu) && !UpdateMainMenuUI) ||
                (Info && (Info->Capabilities & CE_UI_InGame) && !UpdateInGameUI) ||
                (Info && (Info->Capabilities & CE_UI_Pause_Menu) && !UpdatePauseMenuUI) ||
                (Info && (Info->Capabilities & CE_InGame) && !InGameUpdate) ||
                (Info && (Info->Capabilities & CE_Lua_Injects) && !LuaFuncReg)) 
            {
                const char* CapName = nullptr;
                if (Info->Capabilities & CE_UI_Main_Menu) CapName = "MAIN MENU UI";
                else if (Info->Capabilities & CE_UI_InGame) CapName = "IN-GAME UI";
                else if (Info->Capabilities & CE_UI_Pause_Menu) CapName = "PAUSE MENU UI";
                else if (Info->Capabilities & CE_InGame) CapName = "IN-GAME EXECUTION";
                else if (Info->Capabilities & CE_Lua_Injects) CapName = "LUA FUNCTION INJECTS";

                CE::PluginAPI::Log(CE_LOG_WARN,
                    "THE MODULE: %s HAS THE CAPABILITY FOR %s BUT DOES NOT HAVE THE REQUIRED EXPORT.\nPLEASE ADD THE FUNCTION OR REMOVE THE CAPABILITY",
                    Info->Name, CapName
                );
                if(!CE::Debug) CLOSE_LIB(Lib);
                continue;
            }


            LoadedPlugin Plugin{};
            Plugin.handle = Lib;
            Plugin.info = Info;
            Plugin.Init = InitFunc;
            Plugin.Update = UpdateFunc;
            Plugin.LuaInject = LuaFuncReg;
            Plugin.InGameUpdate = InGameUpdate;

            Plugin.UpdateUI_InGameMenu = UpdateInGameUI;
            Plugin.UpdateUI_MainMenu = UpdateMainMenuUI;
            Plugin.UpdateUI_PauseMenu = UpdatePauseMenuUI;

            Plugin.Shutdown = ShutdownFunc;

            Plugin.Init(&Funcs, &GPluginGlobals);

            g_Plugins.push_back(Plugin);

            std::cout << "INFO: CE-PluginLoader: Loaded plugin: " << Info->Name << "\n";
        }

        if (!g_Plugins.empty()) {
            std::cout << "\nINFO: Loaded plugins:\n"; // Fancy lookin heheh
            for (const auto &P : g_Plugins) {
                const char *Name = (P.info && P.info->Name) ? P.info->Name : "<unknown>";
                const char *Description = (P.info && P.info->Description) ? P.info->Description : "<unknown>";
                uint32_t VerMajor = (P.info) ? P.info->Plugin_Version_Major : 0;
                uint32_t VerMinor = (P.info) ? P.info->Plugin_Version_Minor : 0;
                uint32_t Caps = (P.info) ? P.info->Capabilities : 0;

                std::cout << "INFO:     > " << Name << "\n";
                std::cout << "INFO:         > Description: " << Description << "\n";
                std::cout << "INFO:         > Version: " << VerMajor << "." << VerMinor << "\n";

                std::cout << "INFO:         > Capabilities: ";
                bool first = true;
                for (const auto& entry : g_CapabilityTable) {
                    if (Caps & entry.flag) {
                        if (!first) std::cout << ", ";
                        std::cout << entry.name;
                        first = false;
                    }
                }
                if (first) std::cout << "None";
                std::cout << "\n";

                std::cout << "INFO:         > Status: Loaded\nINFO:\n";
            }
        }

        if (g_Plugins.empty()) {
            std::cout << "INFO: PluginLoader: No plugins found, the plugin was missing a required export!\n";
            return;
        }
    }

    void Update() {
        for (auto& Plugin : g_Plugins) {
            if (Plugin.Update)
                Plugin.Update();
        }
    }

    void UpdateMainMenuUI() {
        for (auto& Plugin : g_Plugins) {
            if (Plugin.UpdateUI_MainMenu) {
                Plugin.UpdateUI_MainMenu();
            }     
        }
    }

    void UpdateInGameUI() {
        for (auto& Plugin : g_Plugins) {
            if (Plugin.UpdateUI_InGameMenu) {
                Plugin.UpdateUI_InGameMenu();
            }     
        }
    }

    void UpdatePauseMenuUI() {
        for (auto& Plugin : g_Plugins) {
            if (Plugin.UpdateUI_PauseMenu) {
                Plugin.UpdateUI_PauseMenu();
            }     
        }
    }

    void UpdateInGame() {
        for (auto& Plugin : g_Plugins) {
            if (Plugin.InGameUpdate) {
                Plugin.InGameUpdate();
            }     
        }
    }

    void Shutdown() {
        for (auto& Plugin : g_Plugins) {
            if (Plugin.Shutdown)
                Plugin.Shutdown();

            CLOSE_LIB(Plugin.handle);
        }

        g_Plugins.clear();
    }
}