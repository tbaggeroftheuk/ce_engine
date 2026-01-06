#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_TEXTURES 200

#define PATH_MAX_LEN 400
#define GAME_TITLE_LEN 205
#define GAME_VER_LEN 100

#if defined(_WIN32)
    #define ENGINE_BUILT_ON "Windows"
#elif defined(__linux__)
    #define ENGINE_BUILT_ON "Linux"
#elif defined(__APPLE__) && defined(__MACH__)
    #define ENGINE_BUILT_ON "macOS"
#elif defined(__ANDROID__)
    #define ENGINE_BUILT_ON "Android"
#else
    #define ENGINE_BUILT_ON "Unknown OS"
#endif


#define ENGINE_COMPILER __VERSION__

#define ENGINE_VERSION_STRING "ENGINE ALPHA 1.1"
#define ENGINE_BUILD_INFO ENGINE_VERSION_STRING " | Built: " __DATE__ " " __TIME__ " | Compiler: " ENGINE_COMPILER " | Built on OS: " ENGINE_BUILT_ON

#include <stdbool.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Global stuff's (i just dumped shit here which I don't know a catgory for)

typedef struct {
    // Path stuff
    char startup_video[PATH_MAX_LEN];
    char path[PATH_MAX_LEN];

    lua_State *Lua;
   // Font stuff
    char main_font[PATH_MAX_LEN];
    char main_font_path[PATH_MAX_LEN];
    Font main_font_data;
    
    // Window stuff
    const int window_width;
    const int window_height;
    char window_icon[PATH_MAX_LEN];
    bool should_main_menu;  
    const char game_title[GAME_TITLE_LEN];
	const char version[GAME_VER_LEN];

    bool debug;
} g_globals;

typedef struct {

    char settings_dir[PATH_MAX_LEN];
    char settings_file_path[PATH_MAX_LEN]; 

    // Window settings
    bool fullscreen;

    // Audio settings
    int master_volume;
    int music_volume;
    int sfx_volume;

} g_settings;

extern g_globals ce_globals;

extern g_settings ce_settings;


#endif
