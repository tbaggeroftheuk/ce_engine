#include <iostream>
#include <string>

#include "common/console_link.hpp"
#include "third_party/raylib_cpp/raylib-cpp.hpp"
#include "globals.hpp"
#include "bootstrap.hpp"

int main(int argc, char *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--debug") {
        CE::debug = true;
    }

    if (CE::debug) {
        OpenDebugConsole();
        SetTraceLogLevel(LOG_ALL);
        TraceLog(LOG_INFO, "CE: Debug is currently activated");
    } else {
        SetTraceLogLevel(LOG_NONE);
    }

    TraceLog(LOG_INFO, "CE: Cattle engine: No BULLSHIT");
    TraceLog(LOG_INFO, "CE: Engine version: %s", CE::engine_ver.c_str());

    CE::Bootstrap(); // Actually setting up the files, window and other stuff

    return 0;
}
