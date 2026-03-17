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

        std::string lastState = CE::currentGameStateName;
        if (lastState.empty()) lastState = "None";

        CE::Callbacks::SetGameState(lastState.c_str());
        CE::Callbacks::Emit("Enter");
        CE::Callbacks::Emit(lastState.c_str());

        while(!WindowShouldClose()) 
        {   
            if (CE::currentGameStateName.empty()) CE::currentGameStateName = "None";

            if (CE::currentGameStateName != lastState) {
                CE::Callbacks::SetGameState(lastState.c_str());
                CE::Callbacks::Emit("Exit");

                lastState = CE::currentGameStateName;
                CE::Callbacks::SetGameState(lastState.c_str());
                CE::Callbacks::Emit("Enter");
                CE::Callbacks::Emit(lastState.c_str());
            }

            BeginDrawing();
            ClearBackground(WHITE);
            CE::MousePos = GetMousePosition();

            CE::Callbacks::SetGameState(CE::currentGameStateName.c_str());
            CE::Callbacks::Emit("Update");
            CE::Lua::LuaUpdate();
            CE::Assets::Audio::UpdateMusic();
            CE::Callbacks::Emit("Draw");

            rlImGuiBegin();  // start ImGui frame
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
