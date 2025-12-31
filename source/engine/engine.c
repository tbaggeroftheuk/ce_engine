#include "raylib.h"

#include "globals.h"
#include "engine/engine.h"

void ce_engine_main(void) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Engine is running...", 0, 0, 10, BLACK);

        EndDrawing();
    }
}