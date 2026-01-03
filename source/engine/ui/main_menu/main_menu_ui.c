#include <raylib.h>
#include <stdio.h>

// Third-party 
#include "minini/minIni.h"
#include "raygui.h"

#include "engine/ui/main_menu/main_menu_ui.h"
#include "engine/tex_man/texture_manager.h"
#include "globals.h"
#include "bootstrap.h"

void render_main_menu_ui(void) {
    if (!ce_globals.should_main_menu) return;

    Texture2D testing = CETextures_Get("main_bg");
    DrawTexture(testing, 0, 0,  WHITE);

    int btn_width = 250;
    int btn_height = 50;
    int spacing = 10;
    int start_x = ce_globals.window_width / 2 - btn_width / 2;
    int start_y = ce_globals.window_height / 2 - 60; // starting point for first button

    if (GuiButton((Rectangle){ start_x, start_y, btn_width, btn_height }, "testin")) {
        TraceLog(LOG_INFO, "btn pressed 1 pressed");
    }

    if (GuiButton((Rectangle){ start_x, start_y + btn_height + spacing, btn_width, btn_height }, "Settings")) {
        TraceLog(LOG_INFO, "CE UI: Main menu settings button has been pressed");
    }

    if (GuiButton((Rectangle){ start_x, start_y + (btn_height + spacing) * 2, btn_width, btn_height }, "Quit")) {
        TraceLog(LOG_INFO, "CE UI: Main menu quit button has been pressed");
        ce_exit_global();
    }

}