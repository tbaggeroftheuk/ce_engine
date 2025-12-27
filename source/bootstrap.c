#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bootstrap.h"
#include "globals.h"
#include "tcf.h"
#include "utils.h"
#include "ui/info_box.h"
#include "utils/dir_remove.h"

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
#endif


void setup_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        SDL_Quit();
        abort();
    }
}

void extract_game_data(void) {
    int tmp_dir_random = bag_rand(147483647, 2147483647);
    int goober;

    snprintf(ce_globals.path, sizeof(ce_globals.path),
             "/tmp/ZmFsbGluZ19wb3RhdG9fZ2FtZXM/%d", tmp_dir_random);

    mkdir(ce_globals.path, 0755);

    goober = tcf_extract("data.tcf", ce_globals.path);
    if (goober != TCF_OK) {
        fprintf(stderr, "Fatal Error: %d\n", goober);
        exit(1);
    }
}

void check_boot_vid(void) {
    char combined_path[256];

    snprintf(combined_path, sizeof(combined_path),
             "%s/media/startup.tgc", ce_globals.path);

    if (!file_exists(combined_path)) {
        show_error_box("Fatal Error!", "Couldn't find startup media");
        exit(1);
    }
    strcpy(ce_globals.startup_video, "startup.tgc");
}

void setup_window(void) {
    ce_globals.window = SDL_CreateWindow(
        ce_globals.game_title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    if (ce_globals.window == NULL) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    ce_globals.renderer = SDL_CreateRenderer(
        ce_globals.window, -1, SDL_RENDERER_ACCELERATED
    );

    if (ce_globals.renderer == NULL) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(ce_globals.window);
        SDL_Quit();
        exit(1);
    }
}

void remove_tmp(void) {
    remove_directory(ce_globals.path);
}

void ce_exit(void) {
    SDL_DestroyRenderer(ce_globals.renderer);
    SDL_DestroyWindow(ce_globals.window);
    SDL_Quit();
    remove_tmp();
    exit(0);
}

void ce_bootstrap(void) {
    setup_sdl();
    extract_game_data();
    check_boot_vid();
    setup_window();
}
