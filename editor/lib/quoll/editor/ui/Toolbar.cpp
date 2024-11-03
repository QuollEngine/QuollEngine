#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "StyleStack.h"
#include "Theme.h"
#include "Toolbar.h"
#include "Widgets.h"

namespace quoll::editor {

Toolbar::Toolbar() {
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, Height));
  ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  mOpen = ImGui::Begin("Toolbar", 0,
                       ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoResize |
                           ImGuiWindowFlags_NoSavedSettings |
                           ImGuiWindowFlags_NoDocking);
}

Toolbar::~Toolbar() {
  ImGui::End();
  ImGui::PopStyleVar();
}

bool Toolbar::item(String label, String icon, bool active) {
  StyleStack stack;
  if (active) {
    const auto &imguiCol = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
    const glm::vec4 buttonColor{imguiCol.x, imguiCol.y, imguiCol.z, imguiCol.w};

    stack.pushColor(ImGuiCol_Button, buttonColor);
    stack.pushColor(ImGuiCol_ButtonActive, buttonColor);
    stack.pushColor(ImGuiCol_ButtonHovered, buttonColor);
  }

  const bool clicked = widgets::Button(icon.c_str());

  ImGui::SameLine();
  return clicked;
}

} // namespace quoll::editor
