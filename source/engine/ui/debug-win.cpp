#include "third_party/imgui/imgui.h"
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
#include "engine/assets/assets.hpp"
#include "common/MemoryUsage.hpp"
#include "engine/callbacks.hpp"

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
    ImGui::Spacing();

    float CurrentFPS = static_cast<float>(GetFPS());

    if (CurrentFPS > 5.0f && CurrentFPS < minFps) minFps = CurrentFPS;
    if (CurrentFPS > highFps) highFps = CurrentFPS;
    if (highFps > static_cast<float>(CE::MaxFPS)) highFps = static_cast<float>(CE::MaxFPS);

    avgFps = CurrentFPS;

    ImGui::Spacing();
    ImGui::Text("Max FPS: %i", CE::MaxFPS);
    ImGui::Spacing();
    ImGui::Text("Avg FPS: %.2f", avgFps);
    ImGui::Spacing();
    ImGui::Text("Min FPS: %.2f", minFps);
    ImGui::Spacing();
    ImGui::Text("High FPS: %.2f", highFps);
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

void showLoadedTextures() {
    ImGui::Spacing();
    ImGui::Text("Current error textures: %i", CE::Assets::Textures::LoadedTexturesError());
    ImGui::Text("Current loaded textures: %i", CE::Assets::Textures::LoadedTexturesNoError());
    ImGui::Text("Current loaded textures in total: %i", CE::Assets::Textures::LoadedTextures());
}

void showMemoryUsage() {
    size_t stackLimit = MemoryUsage::getStackLimit();
    size_t heapUsed   = MemoryUsage::getHeapUsage();
    size_t resident   = MemoryUsage::getResidentMemory();
    double stackMB    = stackLimit / (1024.0 * 1024.0);
    double heapMB     = heapUsed / (1024.0 * 1024.0);
    double residentMB = resident / (1024.0 * 1024.0);

    ImGui::Text("Stack Limit: %.2f MB", stackMB);
    ImGui::Text("Heap / Private Memory: %.2f MB", heapMB);
    ImGui::Text("Resident Memory (RAM): %.2f MB", residentMB);
}

void DebugUI() {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Cattle Debug");

    ImGui::Text("Current mousePos: x = %f, y = %f", CE::MousePos.x, CE::MousePos.y);
    ImGui::Text("Current state: %s", CE::Callbacks::GetState());
    ImGui::Spacing();
    ImGui::Text("Game name: %s", CE::game_name.c_str());
    ImGui::Text("Engine version string: %s", CE::engine_ver.c_str());
    ImGui::Text("Engine version int: %i", CE::int_engine_ver);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::BeginTabBar("DebugTabs"))
    {
        if (ImGui::BeginTabItem("FPS"))
        {
            showFPS();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Assets"))
        {
            showLoadedTextures();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Flags"))
        {
            showFlags();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Paths"))
        {
            showGlobals();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Memory"))
        {
            showMemoryUsage();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}