#include <iostream>
#include "globals.hpp"

#include "bootstrap.hpp"
extern "C" {
    #include <raylib.h>
}

int main() {
    TraceLog(LOG_INFO, "CE: Cattle engine: No BULLSHIT");
    TraceLog(LOG_INFO, "CE: Engine version: %s", CE::engine_ver.c_str());
    
    if(CE::debug) {
        SetTraceLogLevel(LOG_ALL);
    } else {
        SetTraceLogLevel(LOG_NONE);
    }
    
    CE::extract_game();
    CE::window_init();


    return 0;
}