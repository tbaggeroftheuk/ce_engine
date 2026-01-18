#include <iostream>

#include "third_party/raylib_cpp/raylib-cpp.hpp"

#include "globals.hpp"
#include "bootstrap.hpp"

int main() {
    TraceLog(LOG_INFO, "CE: Cattle engine: No BULLSHIT");
    TraceLog(LOG_INFO, "CE: Engine version: %s", CE::engine_ver.c_str());
    
    if(CE::debug) {
        SetTraceLogLevel(LOG_ALL);
    } else {
        SetTraceLogLevel(LOG_NONE);
    }

    CE::Bootstrap(); // Actually setting up the files, window and other shit

    return 0;
}