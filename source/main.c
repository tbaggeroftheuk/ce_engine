#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void Run_exe() { //just a test
    clear_screen();
    printf("This is a stub oh and it stop the background music as a test\n");
    stop_mp3(true);
    pause_ms(2000);
}

void doom() { 
    clear_screen();
    printf("Erm I can't be bothered to launch doom.\n");
    printf("Enjoy this smiley face :)\n");
    pause_ms(2000);
    clear_screen();
}

void test() {
    int test_number; //set varible 

    clear_screen();
    printf("Enter the answer to all: ");
    scanf("%d", &test_number);

    if (test_number == 42) { // test if its 42
        clear_screen();
        printf("The answer to all is 42\n");
        pause_ms(2000);
    }
    else {
        clear_screen();
        printf("%d is not the answer to all.\n", test_number);
        pause_ms(2000);
    }
}

void write_a_file() {
    clear_screen();
    printf("So theres a 99\n");

    char current_filename[100];
    char current_file_contents[999];

    // flush leftover newline in case scanf or something ran before
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    printf("Enter the filename: ");
    fgets(current_filename, sizeof(current_filename), stdin);
    current_filename[strcspn(current_filename, "\n")] = '\0'; // removes newline

    printf("\nEnter the file contents: ");
    fgets(current_file_contents, sizeof(current_file_contents), stdin);
    current_file_contents[strcspn(current_file_contents, "\n")] = '\0'; // removes newline

    file_write(current_filename, current_file_contents);
}

void calculate() { // No idea if there is efficent way to pick operation but eh it works
    clear_input_buffer();
    clear_screen();
    char current_operation = input_char("Enter operation +, -, *, /: ");
    double num1 = input_double("\nEnter da first number:");
    double num2 = input_double("\nEnter the second number:");
    double result;
    if (current_operation == '+') {
        result = num1 + num2;
    } else if (current_operation == '-') {
        result = num1 - num2;
    } else if (current_operation == '*') {
        result = num1 * num2;
    } else if (current_operation == '/') {
        if (num2 == 0) {
            printf("Cant divide by zero\n");
            pause_ms(2000);
            return;
        }
        result = num1 / num2;
    } else {
        printf("Unknown operation '%c'\n", current_operation);
        pause_ms(2000);
        return;
    }
    printf("Result: %g\n", result);
    pause_ms(2000);
}

void mp3_player() {
    clear_input_buffer();
    clear_screen();
    char current_mp3_name[1000000];
    input_string("Enter the name of the mp3: ", current_mp3_name, sizeof(current_mp3_name));
    play_mp3(current_mp3_name, false);
}

int main() {
    int Menu_pick;
    play_mp3("song.mp3", true);

    while (1) { // infinite loop to keep showing the menu
        clear_screen();
        printf("\x1b[5m\x1b[44m TBag Microsystems \x1b[5m\n");
        printf("\x1b[0mPlease enter your choice\n\n");
        printf("\x1b[101m1: Run an exe.\x1b[31m\n"); 
        printf("\x1b[0m\x1b[102m2: Play doom.\x1b[0m\n");
        printf("\x1b[45;37m3: Find the answer to life, the universe and everything\n");
        printf("\x1b[46;37m4: Write a file\x1b[0m\n");
        printf("5: Use my calculator :D\n");
        printf("6: Change background music\n");
        printf("\x1b[42;37m0: Exit\n\n");
        printf("\nThis is now on github!\n");
        printf("\x1b[0mEnter your choice: ");
        scanf("%d", &Menu_pick);

        if (Menu_pick == 1) {
            Run_exe();
        } else if (Menu_pick == 2) {
            doom();
        } else if (Menu_pick == 3) {
            test();
        } else if (Menu_pick == 4) {
            write_a_file();
        } else if (Menu_pick ==5) {
            calculate();
        } else if (Menu_pick == 0) {
            printf("Exiting...\n");
            break; // exit the loop
        } else if (Menu_pick == 6)  {
            mp3_player();
        } else {
            printf("Invalid option\n");
        }
    }

    return 0;
}
