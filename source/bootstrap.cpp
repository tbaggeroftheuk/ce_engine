#include <cstdlib>
#include <format>
#include <string>

#include "raylib-cpp.hpp"

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
                std::exit(0);
            }
            std::string Home(home_cstr);
            CE::Global.data_path = std::format("{}/.cache/{}", Home, CE::game_name);

        #elif __APPLE__
            const char* home_cstr = std::getenv("HOME");
            if (!home_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find user home directory");
                std::exit(0);
            }
            std::string Home(home_cstr);
            CE::Global.data_path = std::format("{}/Library/Caches/{}", Home, CE::game_name);

        #elif _WIN32
            const char* localAppData_cstr = std::getenv("LOCALAPPDATA");
            if (!localAppData_cstr) {
                TraceLog(LOG_ERROR, "CE: Can't find LOCALAPPDATA environment variable");
                std::exit(0);
            }
            std::string Home(localAppData_cstr);
            CE::Global.data_path = std::format("{}/{}/Cache", Home, CE::game_name);
        #endif

        if(!DirectoryExists(CE::Global.data_path.c_str())) {
            if(!MakeDirectory(CE::Global.data_path.c_str())) {
                TraceLog(LOG_ERROR, "CE: Can't create cache directory");
                std::exit(1);
            }
        }
        TraceLog(LOG_INFO, "CE: Game data is: %s", CE::Global.data_path.c_str());

        // Versioning the cache as probs better than extracting EVERYTIME you run the program
        std::string version_file_path;
        version_file_path = std::format("{}/.version", CE::Global.data_path);

        if(!FileExists(version_file_path.c_str())) {
            TraceLog(LOG_INFO, "CE: Version file missing or not created");
        }
    }
}