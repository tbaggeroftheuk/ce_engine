#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "utils.h"
#include "tcf.h"
#include "math_util.h"
#include "bootstrap.h"


int main(void) {
    srand(time(NULL)); // dont fricking remove
    ce_bootstrap();
    printf("If you see this everything ran correctly!\n");
    return 0;
}
