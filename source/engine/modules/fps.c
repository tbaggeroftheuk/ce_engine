#include <raylib.h>
#include <stdio.h>

#include "engine/modules/CE_fps.h"
#include "globals.h"

void show_fps(void) {
    int current_fps;
    current_fps = GetFPS();
    char fps_text[20];
    snprintf(fps_text, sizeof(fps_text), "FPS: %d", current_fps);
    DrawTextEx(ce_globals.main_font_data, fps_text, (Vector2){10, 10}, 20, 1, BLACK);
}