// External includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

// Internal includes
#include "bootstrap.h"
#include "globals.h" 
// used for ce_globals and ce_settings

// Engine includes
#include "engine/engine.h"
#include "engine/modules/settings/settings.h"
#include "engine/fio/tcf.h"
#include "engine/fio/dir_remove.h"
#include "engine/tex_man/texture_manager.h"

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
#endif


void extract_game_data(void) { // TODO: FINISH THIS
    int tcf;

    #ifdef __linux__
        const char *cache = getenv("HOME");
        if (!cache) {
            TraceLog(LOG_ERROR, "CE: HOME has not been set we cant extract game data :(");
            exit(1);
        }
        snprintf(ce_globals.path, sizeof(ce_globals.path), "%s/.cache/%s", cache, ce_globals.game_title);

    #elif __APPLE__
        snprintf(ce_globals.path, sizeof(ce_globals.path), "%s/Library/Caches/%s", getenv("HOME"), ce_globals.game_title);
    #endif
    
    if(!DirectoryExists(ce_globals.path)) {
        MakeDirectory(ce_globals.path);
    }
    char version_file_path[PATH_MAX_LEN];
    snprintf(version_file_path, sizeof(version_file_path), "%s/.version", ce_globals.path);

    if(!FileExists(version_file_path)) {
        TraceLog(LOG_INFO, "CE: Missing version file, Creating one");
		FILE *version_file_data = fopen(version_file_path, "w");
		if(file == NULL) {
			TraceLog(LOG_INFO, "Can't write version file"); 
			exit(1)
		}
    }


    tcf = tcf_extract("data.tcf", ce_globals.path);
    if (tcf != TCF_OK) {
        TraceLog(LOG_ERROR, "CE: Failed to extract game data!");
        exit(1);
    }
    TraceLog(LOG_INFO, "CE: Extracted game data to %s", ce_globals.path);
}

void check_boot_vid(void) {
    char combined_path[PATH_MAX_LEN];

    snprintf(combined_path, sizeof(combined_path),
             "%s/media/startup.tgc", ce_globals.path);

    if (!FileExists(combined_path)) {
        TraceLog(LOG_ERROR, "CE: Startup video not found!");
        exit(1);
    }
    strcpy(ce_globals.startup_video, "startup.tgc");
    TraceLog(LOG_INFO, "CE: Found startup video!");
}

void setup_window(void) {
    InitWindow(ce_globals.window_width, ce_globals.window_height, ce_globals.game_title);
    if (ce_settings.fullscreen) {
        ToggleFullscreen();
    }
    char window_icon[PATH_MAX_LEN];
    snprintf(window_icon, sizeof(window_icon), "%s/media/%s", ce_globals.path, ce_globals.window_icon);
    SetWindowIcon(LoadImage(window_icon));
    SetTargetFPS(60);
    TraceLog(LOG_INFO, "CE: Window and fps set!");
}

void font_load(void) {
    snprintf(ce_globals.main_font_path, sizeof(ce_globals.main_font_path), "%s/fonts/%s", ce_globals.path, ce_globals.main_font);
    ce_globals.main_font_data = LoadFont(ce_globals.main_font_path);
    TraceLog(LOG_INFO, "CE: Main font loaded from %s", ce_globals.main_font_path);
}

void ce_exit(void) {
    remove_directory(ce_globals.path);
    exit(0);
}

void ce_exit_debug(void) {
    TraceLog(LOG_INFO, "CE: Game has exited");
    exit(0);
}

void ce_exit_global(void) {
    if(ce_globals.debug) {
        ce_exit_debug();
    } else {
        ce_exit();
    }
}

void ce_bootstrap(void) {   
    settings_entrance();
    extract_game_data(); // Must be first! other wise shit breaks
    check_boot_vid();
    
    setup_window();
    font_load();
    ce_engine_main();

}
