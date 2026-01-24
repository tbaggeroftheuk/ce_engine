#include <cstdlib>
#include <format>
#include <string>
#include <fstream>


#include "common/errorbox/error_box.hpp"
#include "engine/engine.hpp"
#include "globals.hpp"

extern "C" {
    #include "common/tcf/tcf.h"
    #include "raylib.h" 
}

namespace CE {
    void extract_game(void) {
        
        #if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

            const char* cache_cstr = std::getenv("XDG_CACHE_HOME");
            const char* home_cstr  = std::getenv("HOME");

            if (!cache_cstr && !home_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find user home directory");
                TraceLog(LOG_ERROR, "CE: Please set an environment variable called HOME or XDG_CACHE_HOME");
                ShowError("CE: Can't find user home directory");
            }

            std::string cache_base = cache_cstr ? cache_cstr : std::format("{}/.cache", home_cstr);
            CE::Global.data_path = std::format("{}/{}", cache_base, CE::game_name);

        #elif __APPLE__

            const char* home_cstr = std::getenv("HOME");
            if (!home_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find user home directory. What did you do to MacOS?!");
                ShowError("CE: Can't find user home directory. What did you do to MacOS?!");
            }
            std::string Home(home_cstr);
            CE::Global.data_path = std::format("{}/Library/Caches/{}", Home, CE::game_name);

        #elif _WIN32

            const char* localAppData_cstr = std::getenv("LOCALAPPDATA");
            if (!localAppData_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find LOCALAPPDATA environment variable");
                ShowError("CE: Can't find LOCALAPPDATA environment variable");
            }
            std::string Home(localAppData_cstr);
            CE::Global.data_path = std::format("{}\\{}\\Cache", Home, CE::game_name);

        #endif

        if (!DirectoryExists(CE::Global.data_path.c_str())) { // Check data path exists
            if (MakeDirectory(CE::Global.data_path.c_str()) != 0) { // Try to make the directory, shows an error if it failed
                TraceLog(LOG_FATAL, "CE-Bootstrap: Unable to create data directory");
                ShowError("CE-Bootstrap: Unable to create data directory");
            }

            TraceLog(LOG_INFO, "CE-Bootstrap: Created data directory");
        }
        TraceLog(LOG_INFO, "CE-Bootstrap: Data directory is %s", CE::Global.data_path.c_str());


        if (!FileExists("data.tcf")) {
            TraceLog(LOG_FATAL, "CE-Bootstrap: Missing game data, please install it!");
            ShowError("CE-Bootstrap: Missing game data, please install it!");
        }

        // Versioned extract as probs better then extracting every run
        bool extract_required = true;
        std::string ver_file_contents = "this_shouldnt_be_viewable";

        std::string ver_file_path = std::format("{}/.version", CE::Global.data_path);

        { // Forcing RAII to close the file
            std::ifstream ver_file(ver_file_path);

            if(!ver_file.is_open()) {
                TraceLog(LOG_ERROR, "CE-Bootstrap: Couldn't open version file for read");
                extract_required = true;
            }else if(!std::getline(ver_file, ver_file_contents)) {
                TraceLog(LOG_INFO, "CE-Bootstrap: Version file is empty");
                extract_required = true;
            }

        }

        if (ver_file_contents != CE::engine_ver) extract_required = true;

        if (CE::debug) extract_required = true;

        TraceLog(LOG_INFO, "CE-Bootstrap: Version from data: %s", ver_file_contents.c_str());

        if (extract_required) {
            int tcf = tcf_extract("data.tcf", CE::Global.data_path.c_str());
            
        switch (tcf) {
            case TCF_ERR_IO:
                TraceLog(LOG_FATAL, "CE-Bootstrap: IO error\n");
                std::exit(1);
                break;

            case TCF_ERR_CRC:
                TraceLog(LOG_FATAL, "CE-Bootstrap: The TCF file has a CRC error\n");
                std::exit(1);
                break;

            case TCF_ERR_MEMORY:
                TraceLog(LOG_FATAL, "CE-Bootstrap: A memory error occurred!\n");
                std::exit(1);
                break;

            case TCF_ERR_FORMAT:
                TraceLog(LOG_FATAL,
                        "CE-Bootstrap: The TCF file has a format error.\n"
                        "Is it a TCF file?\n");
                std::exit(1);
                break;

            default:
                break;
        }



            std::ofstream ver_file_out(ver_file_path);
            if(ver_file_out.is_open()) {
                ver_file_out << CE::engine_ver;
            }
        }

        TraceLog(LOG_INFO, "CE-Bootstrap: Game data has been extracted!");
        return;
    }

    void window_init() {
        InitWindow(CE::Global.window_width, CE::Global.window_height, CE::game_name.c_str()); // changed to standard C raylib

        std::string WindowIconPath = std::format("{}/common/icon.jpg", CE::Global.data_path);
        Image icon = LoadImage(WindowIconPath.c_str()); // standard C raylib
        if (icon.data) {
            SetWindowIcon(icon);
            UnloadImage(icon); // cleanup
        }

        SetTargetFPS(60);
    }

    void Bootstrap(void) {
        extract_game();
        window_init();
        CE::Engine::Main();
    }
}
