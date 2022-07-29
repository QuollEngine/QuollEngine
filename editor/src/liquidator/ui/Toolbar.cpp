#include "liquid/core/Base.h"
#include "Toolbar.h"

#include "liquid/imgui/Imgui.h"

namespace liquidator {

bool Toolbar::begin() {
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, Height));
  ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  return ImGui::Begin("Toolbar", 0,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                          ImGuiWindowFlags_NoResize |
                          ImGuiWindowFlags_NoSavedSettings |
                          ImGuiWindowFlags_NoDocking);
}

void Toolbar::end() {
  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace liquidator
