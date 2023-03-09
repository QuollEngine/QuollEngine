#include "liquid/core/Base.h"

#include "liquid/imgui/Imgui.h"
#include "liquid/imgui/ImguiUtils.h"

#include "TransformOperationControl.h"
#include "StyleStack.h"
#include "Theme.h"
#include "FontAwesome.h"

namespace liquid::editor {

TransformOperationControl::TransformOperationControl(WorkspaceState &state) {

  renderIcon(TransformOperation::Move, state);
  ImGui::SameLine();
  renderIcon(TransformOperation::Rotate, state);
  ImGui::SameLine();
  renderIcon(TransformOperation::Scale, state);
}

void TransformOperationControl::renderIcon(
    TransformOperation transformOperation, WorkspaceState &state) {

  StyleStack stack;
  if (transformOperation == state.activeTransform) {
    const auto &imguiCol = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
    glm::vec4 buttonColor{imguiCol.x, imguiCol.y, imguiCol.z, imguiCol.w};

    stack.pushColor(ImGuiCol_Button, buttonColor);
    stack.pushColor(ImGuiCol_ButtonActive, buttonColor);
    stack.pushColor(ImGuiCol_ButtonHovered, buttonColor);
  }

  if (ImGui::Button(getTransformOperationIcon(transformOperation))) {
    state.activeTransform = transformOperation;
  }
}

const char *TransformOperationControl::getTransformOperationIcon(
    TransformOperation transformOperation) {
  switch (transformOperation) {
  case TransformOperation::Scale:
    return fa::ExpandAlt;
  case TransformOperation::Rotate:
    return fa::Rotate;
  case TransformOperation::Move:
  default:
    return fa::Arrows;
  }
}

} // namespace liquid::editor
