#include "liquid/core/Base.h"
#include "Toolbar.h"

#include "liquid/imgui/Imgui.h"

namespace liquid::editor {

Toolbar::Toolbar() {
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, Height));
  ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  mExpanded = ImGui::Begin(
      "Toolbar", 0,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
          ImGuiWindowFlags_NoDocking);
}

Toolbar::~Toolbar() {
  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace liquid::editor
