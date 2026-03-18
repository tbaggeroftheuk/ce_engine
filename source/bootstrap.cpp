#include <cstdlib>
#include <cstdint>
#include <format>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "engine/common/error_box.hpp" // This is to show an infobox for a critiqual error
#include "engine/engine.hpp" // Get access to CE::Engine::Main()
#include "common/console_link.hpp" // So we can hook imgui console up
#include "engine/assets/assets.hpp" // to init the textures
#include "engine/plugins/plugins.hpp" // to init plugins
#include "engine/lua.hpp" // To int and run lua
#include "globals.hpp"

#include "common/tcf/tcf.h"
#include "common/ini.hpp"
extern "C" {
    #include <raylib.h> 
    #include "third_party/minini/minIni.h"
    #include "third_party/minini/minGlue.h"
}

namespace CE {
    
    void SetupGlobals() {

        if (!FileExists(DATA_FILE_NAME.c_str())) {
            TraceLog(LOG_FATAL, "CE-Bootstrap: Missing game data, please install it!");
            ShowError("CE-Bootstrap: Missing game data, please install it!");
            std::exit(1);
        }

        uint8_t* data = nullptr;
        uint32_t size = 0;

        
        int res = tcf_load_file(DATA_FILE_NAME.c_str(), "Gameinfo.txt", &data, &size);
        if (res != TCF_OK) {
            TraceLog(LOG_ERROR, "CE-Bootstrap: Failed to load Gameinfo.txt from game data. Error code: %i", res);
            ShowError("CE-Bootstrap: Failed to load Gameinfo.txt from game data");
            return;
        }
        CE::Ini::IniFile ini;
        CE::Ini::ParseError err;
        CE::Ini::Options opts;
        opts.allow_inline_comments = true;  
        opts.allow_colon_delim = true;    
        opts.allow_empty_values = true;   

        bool ok = CE::Ini::parse_memory(data, size, ini, &err, opts);
        if (!ok) {
            std::cerr << "INI parse error at line " << err.line
                    << ", column " << err.column
                    << ": " << err.message << "\n";
            tcf_free(data);
            return;
        }

        CE::game_name = ini.get_string("Gameinfo", "Game_Name", "ERROR MISSING STRING");
        CE::game_ver = ini.get_string("Gameinfo", "Game_Version", "ERROR MISSING STRING");
        
        TraceLog(LOG_INFO, "CE-Bootstrap: Game name and version: %s, %s", CE::game_name.c_str(), CE::game_ver.c_str());

        CE::MaxFPS = ini.get_int("Graphics", "Max_FPS", 60);
        CE::Global.window_width = ini.get_int("Graphics", "Window_Width", 1280);
        CE::Global.window_height = ini.get_int("Graphics", "Window_Height", 720);
        return;
    }

void SetupPaths() {
    namespace fs = std::filesystem;

#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    const char* CacheCstr = std::getenv("XDG_CACHE_HOME");
    const char* HomeCstr  = std::getenv("HOME");
    if (!CacheCstr && !HomeCstr) {
        TraceLog(LOG_ERROR, "CE: Can't find user home directory");
        ShowError("CE: Can't find user home directory");
    }
    std::string CacheBase = CacheCstr
        ? CacheCstr
        : std::format("{}/.cache", HomeCstr);

    CE::Global.data_path = std::format("{}/{}", CacheBase, CE::game_name);

#elif __APPLE__
    const char* home_cstr = std::getenv("HOME");
    if (!home_cstr) {
        TraceLog(LOG_ERROR, "CE: Can't find user home directory");
        ShowError("CE: Can't find user home directory");
    }
    CE::Global.data_path =
        std::format("{}/Library/Caches/{}", home_cstr, CE::game_name);

#elif _WIN32
    const char* localAppData_cstr = std::getenv("LOCALAPPDATA");
    if (!localAppData_cstr) {
        TraceLog(LOG_ERROR, "CE: Can't find LOCALAPPDATA environment variable");
        ShowError("CE: Can't find LOCALAPPDATA environment variable");
    }
    CE::Global.data_path =
        std::format("{}\\{}\\Cache", localAppData_cstr, CE::game_name);
#endif

    // --- Settings & save paths (modern C++) ---
#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    if (!HomeCstr) {
        TraceLog(LOG_ERROR, "CE: Can't find user home directory");
        ShowError("CE: Can't find user home directory");
    }

    CE::Global.settings_path =
        std::format("{}/.config/{}", HomeCstr, CE::game_name);

#elif defined(_WIN32)
    const char* userprofile_cstr = std::getenv("USERPROFILE");
    if (!userprofile_cstr) {
        TraceLog(LOG_ERROR, "CE: Can't find USERPROFILE environment variable");
        ShowError("CE: Can't find USERPROFILE environment variable");
    }

    CE::Global.settings_path =
        std::format("{}\\AppData\\Roaming\\{}", userprofile_cstr, CE::game_name);

#elif defined(__APPLE__)
    const char* home_cstr = std::getenv("HOME");
    if (!home_cstr) {
        TraceLog(LOG_ERROR, "CE: Can't find user home directory");
        ShowError("CE: Can't find user home directory");
    }

    CE::Global.settings_path =
        std::format("{}/Library/Application Support/{}",
                    home_cstr, CE::game_name);

#else
    CE::Global.settings_path = "settings";
#endif

    // Save path lives under settings
    CE::Global.save_path =
        (fs::path(CE::Global.settings_path) / "saves").string();


    // --- Ensure directories exist ---
    fs::create_directories(CE::Global.data_path);
    fs::create_directories(CE::Global.settings_path);
    fs::create_directories(CE::Global.save_path);
}

    void ExtractGame(void) {
        if (!DirectoryExists(CE::Global.data_path.c_str())) { // Check data path exists
            if (MakeDirectory(CE::Global.data_path.c_str()) != 0) { // Try to make the directory, shows an error if it failed
                TraceLog(LOG_FATAL, "CE-Bootstrap: Unable to create data directory");
                ShowError("CE-Bootstrap: Unable to create data directory");
            }

            TraceLog(LOG_INFO, "CE-Bootstrap: Created data directory");
        }
        TraceLog(LOG_INFO, "CE-Bootstrap: Data directory is %s", CE::Global.data_path.c_str());


        if (!FileExists(DATA_FILE_NAME.c_str())) {
            TraceLog(LOG_FATAL, "CE-Bootstrap: Missing game data, please install it!");
            ShowError("CE-Bootstrap: Missing game data, please install it!");
        }

        // Versioned extract as probs better then extracting every run
        bool ExtractRequired = true;
        std::string VerFileContents = "this_shouldnt_be_viewable";

        std::string VerFilePath = std::format("{}/.version", CE::Global.data_path);

        { // Forcing RAII to close the file
            std::ifstream VerFile(VerFilePath);

            if(!VerFile.is_open()) {
                TraceLog(LOG_ERROR, "CE-Bootstrap: Couldn't open version file for read");
                ExtractRequired = true;
            }else if(!std::getline(VerFile, VerFileContents)) {
                TraceLog(LOG_INFO, "CE-Bootstrap: Version file is empty");
                ExtractRequired = true;
            }

        }

        if (VerFileContents != CE::game_ver) ExtractRequired = true;

        if (CE::Debug) ExtractRequired = true;

        TraceLog(LOG_INFO, "CE-Bootstrap: Version from data: %s", VerFileContents.c_str());

        if (ExtractRequired) {
            int Tcf = tcf_extract(DATA_FILE_NAME.c_str(), CE::Global.data_path.c_str());
            
        switch (Tcf) {
            case TCF_ERR_IO:
                TraceLog(LOG_FATAL, "CE-Bootstrap: IO error\n");
                ShowError("Error extracting game data");
                std::exit(1);
                break;

            case TCF_ERR_CRC:
                TraceLog(LOG_FATAL, "CE-Bootstrap: The TCF file has a CRC error\n");
                if (CE::Flags::bypass_data_file_crc_crash) {
                    ShowError("Game data may be corrupted, try reinstalling");
                    std::exit(1);
                }
                break;

            case TCF_ERR_MEMORY:
                TraceLog(LOG_FATAL, "CE-Bootstrap: A memory error occurred!\n");
                ShowError("Internal engine error :(");
                std::exit(1);
                break;

            case TCF_ERR_FORMAT:
                TraceLog(LOG_FATAL,
                        "CE-Bootstrap: The TCF file has a format error.\n"
                        "Is it a TCF file?\n");
                ShowError("Game data file may be corrupted, try reinstalling");
                std::exit(1);
                break;

            default:
                break;
        }
            std::ofstream VerFileOut(VerFilePath);
            if(VerFileOut.is_open()) {
                VerFileOut << CE::game_ver;
            }
        }
        TraceLog(LOG_INFO, "CE-Bootstrap: Game data has been extracted!");
        return;
    }

    void WindowInit() {
        InitWindow(CE::Global.window_width, CE::Global.window_height, CE::game_name.c_str()); // changed to standard C raylib

        if(!IsWindowReady()) {
            TraceLog(LOG_FATAL, "CE-Window: Unable to create window");
            ShowError("Unable to create game window");
            std::exit(1);
        }

        std::string WindowIconPath = std::format("{}/common/icon.jpg", CE::Global.data_path);
        Image Icon = ::LoadImage(WindowIconPath.c_str()); // standard C raylib
        if (Icon.data) {
            SetWindowIcon(Icon);
            UnloadImage(Icon); // cleanup
        }

        SetTargetFPS(MaxFPS);

        if (CE::Debug) {
            SetExitKey(KEY_END);
        } else {
            SetExitKey(KEY_NULL);
        }
    }

    void Bootstrap(void) {
        SetupGlobals();
        SetupPaths();
        ExtractGame();
        WindowInit();
        CE::Assets::Textures::Init();
        CE::Assets::Audio::Init();
        CE::Plugins::LoadModules();
        CE::Lua::init();
        CE::Lua::LoadStartup();
        CE::Engine::Main();
    }
}
