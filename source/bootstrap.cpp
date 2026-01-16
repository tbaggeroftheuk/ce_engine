#include <cstdlib>
#include <format>
#include <string>
#include <fstream>

#include "raylib-cpp.hpp"
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
                showError("CE: Can't find LOCALAPPDATA environment variable");
                std::exit(0);
            }
            std::string Home(localAppData_cstr);
            CE::Global.data_path = std::format("{}/{}/Cache", Home, CE::game_name);
        #endif

        if (!DirectoryExists(CE::Global.data_path.c_str())) {
            int result = MakeDirectory(CE::Global.data_path.c_str());
            if (result != 0) {
                TraceLog(LOG_ERROR, "CE: Can't create cache directory");
                ShowError("CE: Can't create cache directory");
                std::exit(1);
            }
        }
        TraceLog(LOG_INFO, "CE: Game data is: %s", CE::Global.data_path.c_str());

        std::string version_file_path = std::format("{}/.version", CE::Global.data_path);

        if (!FileExists(version_file_path.c_str())) {
            TraceLog(LOG_INFO, "CE: Version file missing or not created");
            SaveFileText(version_file_path.c_str(), CE::engine_ver.data());
        }

        std::string ver_file_data;

        {
            std::ifstream ver_file(version_file_path);
            if (!ver_file.is_open()) {
                TraceLog(LOG_ERROR, "CE: Couldn't open version file for read");
                ShowError("CE: Couldn't open version file for read");
                std::exit(1);
            }

            if (!std::getline(ver_file, ver_file_data)) {
                TraceLog(LOG_ERROR, "CE: Version file is empty");
                ShowError("CE: Version file is empty");
            }
        }

        if (CE::debug) {
            int tcf = tcf_extract("data.tcf", CE::Global.data_path.c_str());
            if (tcf != TCF_OK) {
                TraceLog(LOG_ERROR, "CE: Failed to extract game data");
                ShowError("CE: Failed to extract game data");
                std::exit(1);
            }
            TraceLog(LOG_INFO, "CE: Extracted game data to: %s", CE::Global.data_path.c_str());

            std::ofstream ver_file(version_file_path);
            if (!ver_file.is_open()) {
                TraceLog(LOG_ERROR, "CE: Couldn't open version file for write");
                ShowError("CE: Couldn't open version file for write");
                std::exit(1);
            }

            ver_file << CE::engine_ver;
        }

        if (ver_file_data == CE::engine_ver) {
            return;
        } else {
            int tcf = tcf_extract("data.tcf", CE::Global.data_path.c_str());
            if (tcf != TCF_OK) {
                TraceLog(LOG_ERROR, "CE: Failed to extract game data");
                ShowError("CE: Failed to extract game data");
            }
            TraceLog(LOG_INFO, "CE: Extracted game data to: %s", CE::Global.data_path.c_str());

            std::ofstream ver_file(version_file_path);
            if (!ver_file.is_open()) {
                TraceLog(LOG_ERROR, "CE: Couldn't open version file for write");
                ShowError("CE: Couldn't open version file for write");
                std::exit(1);
            }

            ver_file << CE::engine_ver;
        }
        ShowError("CE: Extracted game data!");
        return;
    }
}
