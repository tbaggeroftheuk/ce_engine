#include "globals.hpp"
#include "engine/engine.hpp"
#include "engine/assets/assets.hpp"
#include "engine/plugins/plugins.hpp"
#include "engine/lua.hpp"
#include "engine/ui.hpp" 
#include "engine/callbacks.hpp"
#include "rlImGui.h" 

#include "imgui.h"
extern "C" {
    #include <raylib.h>
}

namespace CE::Engine {
    int Main() {
        TraceLog(LOG_INFO, "CE-Main: Entering main loop");
        rlImGuiSetup(true);
        
        while(!WindowShouldClose()) 
        {  
            float dt = GetFrameTime();
            CE::Callbacks::Update(dt);
            CE::Assets::Audio::UpdateMusic();

            BeginDrawing();
            ClearBackground(WHITE);

            CE::MousePos = GetMousePosition();
            CE::Callbacks::Draw();

            rlImGuiBegin();

            if(CE::Debug) {
                DebugUI();
                DebugConsole();
            }

            rlImGuiEnd(); 

            EndDrawing();

            if(CE::should_exit) {
                break;
            }
        }
        CloseWindow();
        TraceLog(LOG_INFO, "CE-Main: Exited main loop");
        return 0;
    };
}
