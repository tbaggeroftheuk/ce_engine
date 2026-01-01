#include <stdio.h>
#include <time.h>
#include <raylib.h>
#include <stdbool.h>

#include "bootstrap.h"
#include "globals.h"
#include "engine/engine.h"

int main(int argc, char *argv[]) {
    SetRandomSeed(time(NULL)); // Don't fricking remove

    if (argc > 1) { // Should I add this to the release build, probs not but ehh
        if (strcmp(argv[1], "debug-mode") == 0) {
            ce_globals.debug = true;
        }
    }

    if (ce_globals.debug) { // idk if this is easy to break but eh 
        SetTraceLogLevel(LOG_ALL);
        ce_bootstrap();
        ce_exit_debug();
    } else {
        SetTraceLogLevel(LOG_NONE);
        ce_bootstrap();
        ce_exit();
    }
    return 0;
}
