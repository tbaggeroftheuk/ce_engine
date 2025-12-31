// External includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

// Internal includes
#include "bootstrap.h"
#include "globals.h" // used for ce_globals

// Engine includes
#include "engine/engine.h"
#include "engine/fio/tcf.h"
#include "engine/fio/dir_remove.h"

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

    mkdir(ce_globals.path, 0755);

    goober = tcf_extract("data.tcf", ce_globals.path);
    if (goober != TCF_OK) {
        fprintf(stderr, "Fatal Error: %d\n", goober);
        exit(1);
    }
    TraceLog(LOG_INFO, "CE: Extracted game data!");
}

void check_boot_vid(void) {
    char combined_path[256];

    snprintf(combined_path, sizeof(combined_path),
             "%s/media/startup.tgc", ce_globals.path);

    if (!FileExists(combined_path)) {
        fprintf(stderr, "Fatal Error: Missing startup video\n");
        exit(1);
    }
    strcpy(ce_globals.startup_video, "startup.tgc");
    TraceLog(LOG_INFO, "CE: Found startup video!");
}

void setup_window(void) {
    InitWindow(ce_globals.window_width, ce_globals.window_height, ce_globals.game_title);
    SetTargetFPS(30);
    TraceLog(LOG_INFO, "CE: Window and fps set!");
}

void ce_exit(void) {
    remove_directory(ce_globals.path);
    exit(0);
}

void ce_exit_debug(void) {
    TraceLog(LOG_INFO, "CE: Game has exited");
    exit(0);
}

void ce_bootstrap(void) {
    extract_game_data();
    check_boot_vid();
    setup_window();
    ce_engine_main();
}
