#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bootstrap.h"
#include "tcf.h"
#include "utils.h"
#include "ui/info_box.h"


#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
#endif

char path[256];

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
    
    snprintf(path, sizeof(path), "/tmp/ZmFsbGluZ19wb3RhdG9fZ2FtZXM/%d", tmp_dir_random);

    mkdir(path, 0755);

    goober = tcf_extract("data.tcf", path);
    if (goober != TCF_OK) {
        fprintf(stderr, "Fatal Errror: %d\n", goober);
        exit(1);
    }
}

void check_boot_vid(void) {
    char combined_path [256];
    snprintf(combined_path, sizeof(combined_path), "%s/media/startup.mp4", path);

    if (!file_exists(combined_path)) {
         show_error_box("Fatal Error!", "Couldn't find startup media");
         exit(1);
    }
}

void ce_bootstrap(void) {
    setup_sdl();
    extract_game_data();
    check_boot_vid();
    return;
}
