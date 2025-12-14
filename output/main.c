#include <stdio.h>
#include "utils.h"
#include "math_util.h"

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

void test_3(void) {
    file = tcf_extract("archive.tcf", "output")

    if (rc != TCF_OK) {
        printf("Extraction failed: %d\n", rc);
        return 1;
    }
}

int main(void) {
    test_1();
    test_2();
    printf("\nMain!\n");
    return 0;
}
