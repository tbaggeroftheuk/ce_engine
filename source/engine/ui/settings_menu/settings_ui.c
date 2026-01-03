#include <raylib.h>
#include <stdio.h>

// Third-party 
#include "minini/minIni.h"
#include "raygui.h"

#include "engine/ui/settings_menu/settings_ui.h"
#include "globals.h"
#include "bootstrap.h"

void settings_ui(char source_of_call) {
    char gui_style_path[PATH_MAX_LEN];
    snprintf(gui_style_path, sizeof(gui_style_path), "%s/styles/main.rgs", ce_globals.path);
    GuiLoadStyle(gui_style_path);

    int btn_width = 250;
    int btn_height = 50;
    int spacing = 10;
    int start_x = ce_globals.window_width / 2 - btn_width / 2;
    int start_y = ce_globals.window_height / 2 - 60; // starting point for first button

}