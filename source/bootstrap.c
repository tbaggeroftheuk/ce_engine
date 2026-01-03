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


void extract_game_data(void) {
    int tmp_dir_random = GetRandomValue(147483647, 2147483647);
    int goober;

    snprintf(ce_globals.path, sizeof(ce_globals.path),
             "%s/%d", ce_globals.base_path, tmp_dir_random);

    
    MakeDirectory(ce_globals.path);

    goober = tcf_extract("data.tcf", ce_globals.path);
    if (goober != TCF_OK) {
        TraceLog(LOG_ERROR, "CE: Failed to extract game data!");
        exit(1);
    }
    TraceLog(LOG_INFO, "CE: Extracted game data!");
}

void check_boot_vid(void) {
    char combined_path[256];

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
    extract_game_data(); // Must be first! other wise shit breaks
    check_boot_vid();
    settings_entrance();
    
    setup_window();
    font_load();
    ce_engine_main();

}
