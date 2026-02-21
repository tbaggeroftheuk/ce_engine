#include <string>
#include <format>

#include "globals.hpp"

extern "C" {
#include <raylib.h>
#include "third_party/minini/minIni.h"
#include "third_party/minini/minGlue.h"
}
namespace CE::SettingsManager {
    
    void load_settings(void) {
        std::string settings_file_path = std::format("{}/settings.ini", CE::Global.settings_path);
        TraceLog(LOG_INFO, "CE: Loading settings from %s", CE::Global.settings_path.c_str());
        CE::Settings.fullscreen = ini_getbool("Main", "fullscreen", true, settings_file_path.c_str());
        CE::Settings.master_vol = ini_getl("Audio", "master_volume", 100, settings_file_path.c_str());
        CE::Settings.music_vol = ini_getl("Audio", "music_volume", 100, settings_file_path.c_str());
        CE::Settings.sfx_vol = ini_getl("Audio", "sfx_volume", 100, settings_file_path.c_str());
    }

    void settings_init(void) {
        std::string settings_file_path = std::format("{}/settings.ini", CE::Global.settings_path);

        if (!DirectoryExists(CE::Global.settings_path.c_str()) || !FileExists(settings_file_path.c_str())) {
            MakeDirectory(CE::Global.settings_path.c_str()); 
            TraceLog(LOG_INFO, "CE: Settings directory or file not found");
            ini_putbool("Main", "fullscreen", false, settings_file_path.c_str());
            ini_putl("Audio", "master_volume", 100, settings_file_path.c_str());
            ini_putl("Audio", "music_volume", 100, settings_file_path.c_str());
            ini_putl("Audio", "sfx_volume", 100, settings_file_path.c_str());
            TraceLog(LOG_INFO, "CE: Created generic settings");
        }
        load_settings();
    }
}