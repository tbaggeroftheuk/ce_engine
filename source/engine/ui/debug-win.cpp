#include <imgui.h>
#include <format>

#include "engine/ui.hpp"

#include "globals.hpp"

extern "C" {
    #include <raylib.h>
}

void DebugUI() {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Cattle Debug");
    ImGui::Text("Current mousePos: x = %f, y = %f\n", CE::MousePos.x, CE::MousePos.y);
    ImGui::Button("Click me");

    ImGui::End();
}