#include "liquid/core/Base.h"
#include "Toolbar.h"
#include "StyleStack.h"
#include "Theme.h"

#include "liquid/imgui/Imgui.h"

namespace quoll::editor {

void Toolbar::render(WorkspaceState &state, AssetRegistry &assetRegistry,
                     ActionExecutor &actionExecutor) {
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, Height));
  ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleColor(ImGuiCol_WindowBg,
                        Theme::getColor(ThemeColor::Neutral200));
  if (ImGui::Begin("Toolbar", 0,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoSavedSettings |
                       ImGuiWindowFlags_NoDocking)) {

    for (auto &item : mItems) {
      if (item.type == ToolbarItemType::HideWhenInactive &&
          !item.action->predicate(state, assetRegistry)) {
        continue;
      }

      StyleStack stack;
      if (item.type == ToolbarItemType::Toggleable &&
          !item.action->predicate(state, assetRegistry)) {
        const auto &imguiCol = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
        glm::vec4 buttonColor{imguiCol.x, imguiCol.y, imguiCol.z, imguiCol.w};

        stack.pushColor(ImGuiCol_Button, buttonColor);
        stack.pushColor(ImGuiCol_ButtonActive, buttonColor);
        stack.pushColor(ImGuiCol_ButtonHovered, buttonColor);
      }

      if (ImGui::Button(item.icon.c_str())) {
        actionExecutor.execute(std::move(item.action));

        item.action = item.actionCreator->create();
      }

      ImGui::SameLine();
    }
  }

  ImGui::End();
  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
}

void Toolbar::add(ActionCreator *actionCreator, String label, String icon,
                  ToolbarItemType type) {
  auto ptr = actionCreator->create();
  mItems.push_back({std::unique_ptr<ActionCreator>(actionCreator),
                    std::move(ptr), label, icon, type});
}

} // namespace quoll::editor
