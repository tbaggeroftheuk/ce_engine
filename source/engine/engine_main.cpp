#include "globals.hpp"
#include "engine/engine.hpp"
#include "engine/assets/assets.hpp"
#include "engine/plugins/plugins.hpp" 
#include "engine/UI.hpp"
extern "C" {
    #include <raylib.h>
}

namespace CE::Engine {
    int Main() {
        CE::UI::Widgets::Button btn(300, 250, 200, 100, PURPLE, PINK, VIOLET, BLACK, "Click Me");
        TraceLog(LOG_INFO, "CE-Main: Entering main loop");
        while(!WindowShouldClose()) 
        {   
            BeginDrawing();
            CE::MousePos = GetMousePosition();
            
            ClearBackground(RAYWHITE);
            DrawText("Window created and is displaying stuff", 190, 200, 20, LIGHTGRAY);
            CE::Assets::Textures::Draw("doesntexist", 0, 0);
            CE::Assets::Textures::Draw("brick.png", 100, 200);
            btn.update();
            CE::Modules::Update();
            
            EndDrawing();
        }
        CE::Assets::Textures::Shutdown();
        TraceLog(LOG_INFO, "CE-Main: Exited main loop");
        return CE::should_exit;
    };
}