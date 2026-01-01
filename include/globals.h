#ifndef GLOBALS_H
#define GLOBALS_H

#define PATH_MAX_LEN 256
#define GAME_TITLE_LEN 205

#include <stdbool.h>
#include <raylib.h>

// Global stuff's (i just dumped shit here which I don't know a catagory for)

typedef struct {
    // Path stuff
    char startup_video[PATH_MAX_LEN];
    char path[PATH_MAX_LEN];
    char          base_path[PATH_MAX_LEN];

   // Font stuff
    char main_font[PATH_MAX_LEN];
    char main_font_path[PATH_MAX_LEN];
    Font main_font_data;
    
    // Window stuff
    int window_width;
    int window_height;
    char game_title[GAME_TITLE_LEN];

   bool debug;
} g_globals;

// Global texures

typedef struct {
    Texture2D bg_texture; 

} g_textures;

typedef struct {
    // Window settings
    bool window_mode;

    // Audio settings
    int master_volume;
    int music_volume;
    int sfx_volume;

} g_settings;

extern g_globals ce_globals;

extern g_settings ce_settings;

extern g_textures ce_global_textures;

#endif
