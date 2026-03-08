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
            CE::Assets::Textures::Draw("NOEXISTINGS", 100, 100);
            CE::Assets::Textures::Draw("brick.png", 100, 200);


        if (currentGameState == GameState::MainMenu) {
            CE::Plugins::UpdateMainMenuUI();
        } else if (currentGameState == GameState::InGame) {
            CE::Plugins::UpdateInGame();
            CE::Plugins::UpdateInGameUI();
        } else if (currentGameState == GameState::PauseMenu) {
            CE::Plugins::UpdatePauseMenuUI();
        }

            rlImGuiBegin();  // start ImGui frame
            if(CE::Debug) {
                DebugUI();
                DebugConsole();
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