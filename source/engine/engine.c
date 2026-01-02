#include "raylib.h"

#include "globals.h"
#include "engine/engine.h"

#include "engine/modules/CE_fps.h"
#include "engine/ui/main_menu/main_menu_ui.h"

void ce_engine_main(void) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        show_fps();

        render_main_menu_ui();

        EndDrawing();
    }
}