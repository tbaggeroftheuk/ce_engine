#include <stdio.h>
#include <time.h>

#include "bootstrap.h"
#include "globals.h"
#include "engine/engine.h"
#include "ui/tgc.h"

int var_global_setup(void) {
    char game_title[256] = "The hunt for the golden keys!"; 
}

int main(void) {
    srand(time(NULL)); // Don't fricking remove, I need it for bag_rand()
    
    ce_bootstrap(); // Don't try running any sdl shit before this.

    play_boot_video();

    printf("If you see this everything ran correctly!\n");
    return 0;
}
