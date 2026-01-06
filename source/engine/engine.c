#include <raylib.h>
#include <raygui.h>
#include <stdio.h>

#include "globals.h"
#include "engine/engine.h"
#include "engine/tex_man/texture_manager.h"

#include "engine/modules/CE_fps.h"
#include "engine/ui/main_menu/main_menu_ui.h"

void ce_engine_main(void) {
    CETextures_Init();
    CETextures_Load("main_bg", "media/WPP.png");
	TraceLog(LOG_INFO, "entered the main loop!");
    char gui_style_path[PATH_MAX_LEN];
    snprintf(gui_style_path, sizeof(gui_style_path), "%s/styles/main.rgs", ce_globals.path);
    GuiLoadStyle(gui_style_path);

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(GRAY);

        render_main_menu_ui();

        show_fps();

        EndDrawing();
    }
}