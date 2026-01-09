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
   
    Vector2 mousePos = GetMousePosition();
    Texture2D testing = CETextures_Get("main_bg");
    Texture2D discord = CETextures_Get("discord_logo_blurple");
    DrawTexture(testing, 0, 0,  WHITE);

    // For discord button
    DrawTexture(discord, 5, 680, WHITE);
    int x = 5;
    int y = 680;
    int width = discord.width;
    int height = discord.height;
    Rectangle discordRect = { x, y, (float)width, (float)height };

    int btn_width = 250;
    int btn_height = 50;
    int spacing = 10;
    int start_x = ce_globals.window_width / 2 - btn_width / 2;
    int start_y = ce_globals.window_height / 2 - 60; // starting point for first button

    if (GuiButton((Rectangle){ start_x, start_y, btn_width, btn_height }, "New Game")) {
        TraceLog(LOG_INFO, "CE UI: Main menu new game pressed");
        ce_globals.should_main_menu = false;
    }

    if (GuiButton((Rectangle){ start_x, start_y + btn_height + spacing, btn_width, btn_height }, "Load Game")) {
        TraceLog(LOG_INFO, "CE UI: Main menu load game pressed");
    }

    if (GuiButton((Rectangle){ start_x, start_y + (btn_height + spacing) * 2, btn_width, btn_height }, "Settings")) {
        TraceLog(LOG_INFO, "CE UI: Main menu settings button pressed");
    }

    if (GuiButton((Rectangle){ start_x, start_y + (btn_height + spacing) * 3, btn_width, btn_height }, "Quit")) {
        TraceLog(LOG_INFO, "CE UI: Main menu quit pressed");
        ce_exit_global();
    }

    if (CheckCollisionPointRec(mousePos, discordRect)) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }


    if (CheckCollisionPointRec(mousePos, discordRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        OpenURL("https://discord.gg/P9z2sDPm");
    }

}