#include <cstdlib>
#include <format>
#include <string>
#include <fstream>

#include "third_party/raylib_cpp/raylib-cpp.hpp"
#include "common/errorbox/error_box.hpp"

#include "globals.hpp"
extern "C" {
    #include "common/tcf/tcf.h"
}

namespace CE {
    void extract_game(void) {
        
        #ifdef __linux__
            const char* home_cstr = std::getenv("HOME");
            if (!home_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find user home directory");
                ShowError("CE: Can't find user home directory");
                std::exit(0);
            }
            std::string Home(home_cstr);
            CE::Global.data_path = std::format("{}/.cache/{}", Home, CE::game_name);

        #elif __APPLE__
            const char* home_cstr = std::getenv("HOME");
            if (!home_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find user home directory");
                showError("CE: Can't find user home directory");
                std::exit(0);
            }
            std::string Home(home_cstr);
            CE::Global.data_path = std::format("{}/Library/Caches/{}", Home, CE::game_name);

        #elif _WIN32
            const char* localAppData_cstr = std::getenv("LOCALAPPDATA");
            if (!localAppData_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find LOCALAPPDATA environment variable");
                ShowError("CE: Can't find LOCALAPPDATA environment variable");
                std::exit(0);
            }
            std::string Home(localAppData_cstr);
            CE::Global.data_path = std::format("{}/{}/Cache", Home, CE::game_name);
        #endif

        if (!DirectoryExists(CE::Global.data_path.c_str())) {
            if (MakeDirectory(CE::Global.data_path.c_str()) != 0) {
                TraceLog(LOG_FATAL, "CE-Bootstrap: Unable to create data directory");
                ShowError("CE-Bootstrap: Unable to create data directory");
                std::exit(1);
            }
            TraceLog(LOG_INFO, "CE-Bootstrap: Created data directory");
        }
        TraceLog(LOG_INFO, "CE-Bootstrap: Data directory is %s", CE::Global.data_path.c_str());


        // Versioned extract
        bool extract_required = true;
        std::string ver_file_contents = "this_shouldnt_be_viewable";

        std::string ver_file_path = std::format("{}/.version", CE::Global.data_path);

        { // Forcing RAII to close the file
            std::ifstream ver_file(ver_file_path);

            if(!ver_file.is_open()) {
                TraceLog(LOG_FATAL, "CE-Bootstrap: Couldn't open version file for read");
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
            if (tcf != TCF_OK) {
                TraceLog(LOG_FATAL, "CE-Bootstrap: Failed to extract game data");
                ShowError("CE-Bootstrap: Failed to extract game data");
                std::exit(1);
            }

            std::ofstream ver_file_out(ver_file_path);
            if(ver_file_out.is_open()) {
                ver_file_out << CE::engine_ver;
            }
        }

        TraceLog(LOG_INFO, "CE-Bootstrap: Game data has been extracted!");
        return;
    }

    void window_init(void) {
        main_window = raylib::Window( // This mess of CE is to make the game window
        CE::Global.window_width, // You can modify these settings in globals.hpp 
        CE::Global.window_height,
        CE::game_name);
        SetTargetFPS(60);

        std::string WindowIconPath = std::format("{}/common/icon.png", CE::Global.data_path);
        SetWindowIcon(LoadImage(WindowIconPath.c_str()));
    }

    void Bootstrap(void) {
        extract_game();
        window_init();
    }
}
