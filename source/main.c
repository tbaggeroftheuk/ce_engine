#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "utils.h"
#include "tcf.h"
#include "math_util.h"
#include "bootstrap.h"

void test_1(void) {
    double var_1 = 4.0;
    double var_2 = 3.5;

    if (is_greater_than(var_1, var_2)) {
        printf("Test 1: Worked!\n");
    } else {
        printf("Test 1: Didn't work :(\n");
    }
}

void test_2(void) {
    double var_1 = 4.0;
    double var_2 = 3.5;

    if (is_less_than(var_2, var_1)){
        printf("Test 2: Worked!\n");
    } else {
        printf("Test 2: Didn't work :(\n");
    }
}

void test_3(void){
    int rc;
    rc = tcf_extract("data.tcf", "extracted");

    if (rc != TCF_OK) {
        printf("Extraction failed: %d\n", rc);
        return;
    }
    printf("Extraction succeeded\n");
}

int main(void) {
    srand(time(NULL)); // dont fricking remove
    test_1();
    test_2();
    test_3();
    extract_game_data();
    printf("\nMain!\n");
    return 0;
}
