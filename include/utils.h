#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>   // for size_t
#include <string.h>
#include <stdbool.h> 
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// --- Utility function declarations ---
void pause_ms(int milliseconds);
void clear_screen();
void file_write(const char *filename, const char *file_contents);
void clear_input_buffer();
int input_int(const char *prompt);
double input_double(const char *prompt);
char input_char(const char *prompt);
void input_string(const char *prompt, char *output, size_t size);
int play_mp3(const char *filename, bool verbose);
int stop_mp3(bool verbose);
int bag_rand(int a, int b);
#endif // UTILS_H
