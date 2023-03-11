#include "liquid/core/Base.h"
#include "Toolbar.h"
#include "StyleStack.h"

#include "liquid/imgui/Imgui.h"

namespace liquid::editor {

void Toolbar::render(WorkspaceState &state, ActionExecutor &actionExecutor) {
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, Height));
  ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  if (ImGui::Begin("Toolbar", 0,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoSavedSettings |
                       ImGuiWindowFlags_NoDocking)) {

    for (const auto &item : mItems) {
      if (item.type == ToolbarItemType::HideWhenInactive &&
          !item.action.predicate(state)) {
        continue;
      }

      StyleStack stack;
      if (item.type == ToolbarItemType::Toggleable &&
          item.action.predicate(state)) {
        const auto &imguiCol = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
        glm::vec4 buttonColor{imguiCol.x, imguiCol.y, imguiCol.z, imguiCol.w};

        stack.pushColor(ImGuiCol_Button, buttonColor);
        stack.pushColor(ImGuiCol_ButtonActive, buttonColor);
        stack.pushColor(ImGuiCol_ButtonHovered, buttonColor);
      }

      if (ImGui::Button(item.icon.c_str())) {
        actionExecutor.execute(item.action);
      }

      ImGui::SameLine();
    }
  }

  ImGui::End();
  ImGui::PopStyleVar();
}

void Toolbar::add(const Action &action, String label, String icon,
                  ToolbarItemType type) {
  mItems.push_back({action, label, icon, type});
}

} // namespace liquid::editor
