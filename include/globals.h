#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL.h>

#define PATH_MAX_LEN 256
#define GAME_TITLE_LEN 205

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    char          startup_video[PATH_MAX_LEN];
    char          path[PATH_MAX_LEN];
    char          game_title[GAME_TITLE_LEN];
} g_globals;

extern g_globals ce_globals;

#endif
