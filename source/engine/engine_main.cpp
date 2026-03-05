#include "globals.hpp"
#include "engine/engine.hpp"
#include "engine/assets/assets.hpp"
#include "engine/plugins/plugins.hpp"
#include "engine/ui.hpp" 
#include "rlImGui.h" 

#include <imgui.h>
extern "C" {
    #include <raylib.h>
}

namespace CE::Engine {
    int Main() {
        TraceLog(LOG_INFO, "CE-Main: Entering main loop");
        rlImGuiSetup(true);
        while(!WindowShouldClose()) 
        {   
            BeginDrawing();
            ClearBackground(WHITE);
            CE::MousePos = GetMousePosition();
            
            if (CE::Gamestate::MainMenu != CE::Gamestate::None) {
                CE::Plugins::UpdateMainMenuUI();
            } else if (CE::Gamestate::InGame != CE::Gamestate::None ) {
                CE::Plugins::UpdateInGame();
                CE::Plugins::UpdateInGameUI();
            } else if (CE::Gamestate::PauseMenu != CE::Gamestate::None ) {
                CE::Plugins::UpdatePauseMenuUI();
            }

            rlImGuiBegin();  // start ImGui frame
            if(CE::Debug) {
                DebugUI();
            }
            rlImGuiEnd(); 

            EndDrawing();
        }
        CE::Assets::Textures::Shutdown();
        CloseWindow();
        TraceLog(LOG_INFO, "CE-Main: Exited main loop");
        return CE::should_exit;
    };
}