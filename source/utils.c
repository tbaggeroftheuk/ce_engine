#include "utils.h"  // include your own header
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// Utility functions start

void pause_ms(int milliseconds) {
    SDL_Delay(milliseconds);
}

void clear_screen() {
    printf("\033[H");  // move cursor to top-left
    printf("\033[J");  // clear from cursor to end
    fflush(stdout);    // flush to ensure screen clears now
}

void file_write(const char *filename, const char *file_contents) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    fprintf(fp, "%s", file_contents);
    fclose(fp);
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int input_int(const char *prompt) {
    int value;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input error. Try again.\n");
            continue;
        }
        if (sscanf(buffer, "%d", &value) == 1)
            return value;
        else
            printf("Invalid integer. Try again.\n");
    }
}

double input_double(const char *prompt) {
    double value;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input error. Try again.\n");
            continue;
        }
        if (sscanf(buffer, "%lf", &value) == 1)
            return value;
        else
            printf("Invalid number. Try again.\n");
    }
}

char input_char(const char *prompt) {
    char buffer[10];
    printf("%s", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        return buffer[0];
    return '\0';
}

void input_string(const char *prompt, char *output, size_t size) {
    printf("%s", prompt);
    if (fgets(output, size, stdin) != NULL) {
        output[strcspn(output, "\n")] = '\0';  // remove newline
    } else {
        output[0] = '\0';
    }
}


static Mix_Music *current_music = NULL;

int play_mp3(const char *filename, bool verbose) {
    static int initialized = 0;

    if (!initialized) {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            if (verbose)
                fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
            return -1;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            if (verbose)
                fprintf(stderr, "Mix_OpenAudio Error: %s\n", Mix_GetError());
            SDL_Quit();
            return -1;
        }

        initialized = 1;
    }

    // Stop any currently playing track before starting a new one
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
        if (current_music) {
            Mix_FreeMusic(current_music);
            current_music = NULL;
        }
    }

    current_music = Mix_LoadMUS(filename);
    if (!current_music) {
        if (verbose)
            fprintf(stderr, "Mix_LoadMUS Error: %s\n", Mix_GetError());
        return -1;
    }

    if (Mix_PlayMusic(current_music, 1) == -1) {
        if (verbose)
            fprintf(stderr, "Mix_PlayMusic Error: %s\n", Mix_GetError());
        Mix_FreeMusic(current_music);
        current_music = NULL;
        return -1;
    }

    if (verbose)
        printf("Playing: %s\n", filename);

    return 0;
}

int stop_mp3(bool verbose) {
    if (!Mix_PlayingMusic()) {
        if (verbose)
            printf("No music is currently playing.\n");
        return 0;  // Nothing to stop
    }

    // Stop the music
    Mix_HaltMusic();

    if (verbose)
        printf("Music stopped.\n");

    // Optionally free the music if you want to reclaim memory immediately
    Mix_FreeMusic(Mix_LoadMUS(NULL)); // No-op placeholder; safer way below

    // If you stored the Mix_Music pointer from play_mp3, free it here
    // Example:
    // Mix_FreeMusic(current_music);
    // current_music = NULL;

    return 0;
}

// Utility functions end
