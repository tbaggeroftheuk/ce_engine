#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Thirdparty 
#include <raylib.h>
#include "minini/minIni.h"
#include "minini/minGlue.h"


#include "globals.h"
#include "engine/modules/settings/settings.h"

void settings_init(void) {
    // GOD I LOVE CROSS PLATFORM CODE!
    // even following fucking apple best practices!
    #ifdef __unix__
        snprintf(ce_settings.settings_dir, sizeof(ce_settings.settings_dir), 
        "%s/.config/%s/%s", getenv("HOME"), ce_globals.publisher, ce_globals.game_title);
    #elif defined(_WIN32)
        snprintf(ce_settings.settings_dir, sizeof(ce_settings.settings_dir), 
        "%s/AppData/Roaming/%s/%s", getenv("USERPROFILE"), ce_globals.publisher, ce_globals.game_title);
    #elif __APPLE__
        snprintf(ce_settings.settings_dir, sizeof(ce_settings.settings_dir), 
        "%s/Library/Application Support/%s/%s", getenv("HOME"), ce_globals.publisher, ce_globals.game_title);
    else
        snprintf(ce_settings.settings_dir, sizeof(ce_settings.settings_dir), 
        "%s/saves/%s/%s", ce_globals.base_path, ce_globals.publisher, ce_globals.game_title);
    #endif

    snprintf(ce_settings.settings_file_path, sizeof(ce_settings.settings_file_path),
             "%s/settings.ini", ce_settings.settings_dir);

    if (!DirectoryExists(ce_settings.settings_dir) || !FileExists(ce_settings.settings_file_path)) {
        MakeDirectory(ce_settings.settings_dir); 
        TraceLog(LOG_INFO, "CE: Settings directory or file not found");
        ini_putbool("Main", "fullscreen", false, ce_settings.settings_file_path);
        ini_putl("Audio", "master_volume", 100, ce_settings.settings_file_path);
        ini_putl("Audio", "music_volume", 100, ce_settings.settings_file_path);
        ini_putl("Audio", "sfx_volume", 100, ce_settings.settings_file_path);
        TraceLog(LOG_INFO, "CE: Created generic settings");
    }
}

void load_settings(void) {
    TraceLog(LOG_INFO, "CE: Loading settings from %s", ce_settings.settings_file_path);
    ce_settings.fullscreen = ini_getbool("Main", "fullscreen", true, ce_settings.settings_file_path);
    ce_settings.master_volume = ini_getl("Audio", "master_volume", 100, ce_settings.settings_file_path);
    ce_settings.music_volume = ini_getl("Audio", "music_volume", 100, ce_settings.settings_file_path);
    ce_settings.sfx_volume = ini_getl("Audio", "sfx_volume", 100, ce_settings.settings_file_path);
}

void settings_entrance(void) {
    settings_init();
    load_settings();
}