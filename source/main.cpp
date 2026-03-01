#include <iostream>
#include <string>

#include "common/console_link.hpp"
#include "third_party/raylib_cpp/raylib-cpp.hpp"
#include "globals.hpp"
#include "bootstrap.hpp"

int main(int argc, char *argv[]) {

    // Check for flags
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--debug") { // Enable global logging
            CE::debug = true;
        }

        if(std::string(argv[i]) == "--plugin-logs") { // Show plugin logs
            CE::Flags::show_plugin_logs = true;
        }

        if(std::string(argv[i]) == "--bypass-crc") { // Bypass crc check of the data file
            CE::Flags::bypass_data_file_crc_crash = true;
        }

        if(std::string(argv[i]) == "--log2file") { // Log to a file instead of terminal
            log2file();
        }
    }

    if (CE::debug) {
        OpenDebugConsole();
        SetTraceLogLevel(LOG_ALL);
        TraceLog(LOG_INFO, "CE: Debug is currently activated");
    } else {
        TraceLog(LOG_INFO, "CE Debug is off");
        SetTraceLogLevel(LOG_NONE);
    }

    TraceLog(LOG_INFO, "CE: Cattle engine: No BULLSHIT");
    TraceLog(LOG_INFO, "CE: Engine version: %s", CE::engine_ver.c_str());

    CE::Bootstrap(); // Actually setting up the files, window and other stuff

    return 0;
}
