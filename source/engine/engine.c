#include "raylib.h"

#include "globals.h"
#include "engine/engine.h"

#include "engine/modules/CE_fps.h"

void ce_engine_main(void) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        show_fps();

        EndDrawing();
    }
}