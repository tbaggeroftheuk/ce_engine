#include <iostream>
#include "globals.hpp"

extern "C" {
    #include <raylib.h>
}

int main() {
    TraceLog(LOG_INFO, "Hello, World!");
    if(CE::debug) {
        SetTraceLogLevel(LOG_ALL);
    } else {
        SetTraceLogLevel(LOG_NONE);
    }
    return 1;
}