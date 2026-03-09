#include "third_party/imgui/imgui.h"
#include "engine/ui.hpp"
#include "common/console_link.hpp"

void DebugConsole() {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Console");
    ImGui::TextUnformatted(GetImGuiConsoleText());
    ImGui::End();
}