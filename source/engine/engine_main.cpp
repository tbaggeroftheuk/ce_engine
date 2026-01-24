#include "globals.hpp"
#include "engine/engine.hpp"
#include "engine/assets/assets.hpp"

extern "C" {
    #include <raylib.h>
}

namespace CE::Engine {
    int Main() {
        TraceLog(LOG_INFO, "CE-Main: Entering main loop");
        
        while(!WindowShouldClose()) 
        {   
            BeginDrawing();
            
            ClearBackground(RAYWHITE);
            DrawText("Window created and is displaying stuff", 190, 200, 20, LIGHTGRAY);
            
            EndDrawing();
        }
        TraceLog(LOG_INFO, "CE-Main: Exited main loop");
        return CE::should_exit;
    };
}