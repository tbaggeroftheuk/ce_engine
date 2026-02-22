#include <iostream>
#include <string>

#include "common/console_link.hpp"
#include "third_party/raylib_cpp/raylib-cpp.hpp"
#include "globals.hpp"
#include "bootstrap.hpp"

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--debug") {
            CE::debug = true;
        }

        if(std::string(argv[i]) == "--plugin-logs") {
            CE::showPluginLogs = true;
        }

        if(std::string(argv[i]) == "--log2file") {
            CE::log2file = true;
        }
    }
    if(CE::log2file) {
        log2file();
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
