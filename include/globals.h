#ifndef GLOBALS_H
#define GLOBALS_H

#define PATH_MAX_LEN 256
#define GAME_TITLE_LEN 205

#include <stdbool.h>

typedef struct {
    char          startup_video[PATH_MAX_LEN];
    char          path[PATH_MAX_LEN];
    char          base_path[PATH_MAX_LEN];
    char          game_title[GAME_TITLE_LEN];
    int window_width;
    int window_height;
    bool debug;
} g_globals;

extern g_globals ce_globals;

#endif
