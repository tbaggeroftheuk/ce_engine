#include <imgui.h>
#include <format>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cfloat>

#include "engine/ui.hpp"


extern "C" {
    #include <raylib.h>

}
#include "globals.hpp"

const char* GetGameStateName(CE::GameState state) {
    switch (state) {
        case CE::GameState::None:     return "None";
        case CE::GameState::MainMenu: return "MainMenu";
        case CE::GameState::PauseMenu:return "PauseMenu";
        case CE::GameState::InGame:   return "InGame"; 
        default:                  return "Unknown";
    }
}
float avgFps = 0.00f;
float minFps = 9999.0f; 
float highFps = 0.00f;

void showFPS() {
    int CurrentFPS = GetFPS();
 
    if (CurrentFPS > 5 && CurrentFPS < minFps) minFps = CurrentFPS;
    if (CurrentFPS > highFps) highFps = CurrentFPS;
    if (highFps > CE::MaxFPS) highFps = CE::MaxFPS;

    avgFps = CurrentFPS;

    ImGui::Text("Avg FPS: %f", avgFps);
    ImGui::Text("Min FPS: %f", minFps);
    ImGui::Text("Max FPS: %f", highFps);
}

void showFlags() {
    ImGui::Spacing();
    ImGui::Text("Show plugin logs: %s", CE::Flags::show_plugin_logs ? "True" : "False");
    ImGui::Text("Bypass crc crash: %s", CE::Flags::bypass_data_file_crc_crash ? "True" : "False");
    ImGui::Text("Debug window: %s", CE::Flags::debug_win ? "True" : "False");
}

void showGlobals() {
    ImGui::Spacing();
    ImGui::Text("Data path:");
    if(ImGui::Selectable(CE::Global.data_path.c_str())) ImGui::SetClipboardText(CE::Global.data_path.c_str());
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to copy to clipboard");
    
    ImGui::Spacing();
    ImGui::Text("Settings path:");
    if(ImGui::Selectable(CE::Global.settings_path.c_str())) ImGui::SetClipboardText(CE::Global.settings_path.c_str());
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to copy to clipboard");
    
    ImGui::Spacing();
    ImGui::Text("Saves path:");
    if(ImGui::Selectable(CE::Global.save_path.c_str())) ImGui::SetClipboardText(CE::Global.save_path.c_str());
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to copy to clipboard");
}

void DebugUI() {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Cattle Debug");
    ImGui::Text("Current mousePos: x = %f, y = %f\n", CE::MousePos.x, CE::MousePos.y);
    ImGui::Text("Current state: %s", GetGameStateName(CE::currentGameState));
    showFPS();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Enabled Flags: ");
    showFlags();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Core FS Paths");
    showGlobals();

    ImGui::End();
}