#include "globals.h"
#include <stdbool.h>

g_settings ce_settings = { // Just in case settings load fails or I'm stupid and call smth before loading settings
    .fullscreen     = false,
    .master_volume  = 100,
    .music_volume   = 100,
    .sfx_volume     = 100
};

g_globals ce_globals = {
    .window_width = 800,
    .window_height = 500,
    .game_title = "example",
    .main_font = "Main.ttf",
    .window_icon = "icon.png",
    .should_main_menu = true,
	   .version = ENGINE_BUILD_INFO
    .debug =  true,
};
