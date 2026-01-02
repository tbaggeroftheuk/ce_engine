#include <raylib.h>
#include <stdio.h>

// Thirdparty 
#include "minini/minIni.h"
#include "raygui.h"

#include "engine/ui/main_menu/main_menu_ui.h"
#include "globals.h"

void render_main_menu_ui(void) {
    if (!ce_globals.should_main_menu) return;

    if (GuiButton((Rectangle){ ce_globals.window_width / 2 - 100, ce_globals.window_height / 2 - 60, 200, 40 }, "Example")) {
        TraceLog(LOG_INFO, "btn pressed");
    }
}