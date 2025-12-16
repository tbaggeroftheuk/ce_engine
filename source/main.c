#include <stdio.h>
#include <time.h>

#include "bootstrap.h"
#include "engine/engine.h"


int main(void) {
    srand(time(NULL)); // dont fricking remove, I need it for bag_rand()
    ce_bootstrap();
    printf("If you see this everything ran correctly!\n");
    return 0;
}
