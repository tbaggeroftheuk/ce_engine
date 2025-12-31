#include <stdio.h>
#include <time.h>
#include <raylib.h>
#include <stdbool.h>

#include "bootstrap.h"
#include "globals.h"
#include "engine/engine.h"

int main(void) {
    SetRandomSeed(time(NULL)); // Don't fricking remove
    
    ce_bootstrap(); // STARTUP THE GAME >:D
    

    if (ce_globals.debug) { // idk if this is easy to break but eh 
        ce_exit_debug();
    } else {
        ce_exit();
    }

    printf("If you see this everything ran correctly!\n");
    return 0;
}
