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
        TraceLog(LOG_INFO, "CE-Main: Entering main loop");
        while(!WindowShouldClose()) 
        {   
            BeginDrawing();
            CE::MousePos = GetMousePosition();
            
            if (CE::Gamestate::MainMenu != CE::Gamestate::None) {
                CE::Plugins::UpdateMainMenuUI();
            } else if (CE::Gamestate::InGame != CE::Gamestate::None ) {
                CE::Plugins::UpdateInGame();
                CE::Plugins::UpdateInGameUI();
            } else if (CE::Gamestate::PauseMenu != CE::Gamestate::None ) {
                CE::Plugins::UpdatePauseMenuUI();
            }

            EndDrawing();
        }
        CE::Assets::Textures::Shutdown();
        TraceLog(LOG_INFO, "CE-Main: Exited main loop");
        return CE::should_exit;
    };
}