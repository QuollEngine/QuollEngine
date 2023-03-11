#include "liquid/core/Base.h"

#include "liquid/imgui/Imgui.h"
#include "liquid/imgui/ImguiUtils.h"

#include "TransformOperationControl.h"
#include "StyleStack.h"
#include "Theme.h"
#include "FontAwesome.h"

#include "liquidator/actions/SetActiveTransformActions.h"

namespace liquid::editor {

TransformOperationControl::TransformOperationControl(
    WorkspaceState &state, ActionExecutor &actionExecutor)
    : mActions{SetActiveTransformToMoveAction, SetActiveTransformToRotateAction,
               SetActiveTransformToScaleAction} {

  for (const auto &action : mActions) {
    StyleStack stack;
    if (action.predicate(state)) {
      const auto &imguiCol = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
      glm::vec4 buttonColor{imguiCol.x, imguiCol.y, imguiCol.z, imguiCol.w};

      stack.pushColor(ImGuiCol_Button, buttonColor);
      stack.pushColor(ImGuiCol_ButtonActive, buttonColor);
      stack.pushColor(ImGuiCol_ButtonHovered, buttonColor);
    }

    if (ImGui::Button(String(action.icon).c_str())) {
      actionExecutor.execute(action);
    }
    ImGui::SameLine();
  }
}

} // namespace liquid::editor
