#include <iostream>
#include "globals.hpp"

#include "bootstrap.hpp"
extern "C" {
    #include <raylib.h>
}

int main() {
    TraceLog(LOG_INFO, "CE: Cattle engine! No BULLSHIT :D");
    if(CE::debug) {
        SetTraceLogLevel(LOG_ALL);
    } else {
        SetTraceLogLevel(LOG_NONE);
    }
    
    CE::extract_game();


    return 0;
}